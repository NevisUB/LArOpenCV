#ifndef __DQDXPROFILER_CXX__
#define __DQDXPROFILER_CXX__

#include "dQdXProfiler.h"
#include "PCACandidatesData.h"
#include "DefectClusterData.h"
#include "Core/VectorArray.h"
#include "Core/Geo2D.h"
namespace larocv {

  /// Global larocv::dQdXProfilerFactory to register AlgoFactory
  static dQdXProfilerFactory __global_dQdXProfilerFactory__;

  void dQdXProfiler::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const pca_algo_name = pset.get<std::string>("PCACandidatesName");
    _pca_algo_id = this->ID(pca_algo_name);
    _pi_threshold  = 10;
    _dx_resolution = 3.;
  }
  
  void dQdXProfiler::_Process_(const larocv::Cluster2DArray_t& clusters,
				const ::cv::Mat& img,
				larocv::ImageMeta& meta,
				larocv::ROI& roi)
  {
    auto const& pca_data    = AlgoData<larocv::PCACandidatesData>(_pca_algo_id);
    auto const& defect_data = AlgoData<larocv::DefectClusterData>(pca_data._input_id);
    auto& data = AlgoData<larocv::dQdXProfilerData>();

    // Construct cluster=>line mapping ... elaborated due to how PCACandidates stores info
    // First construct a pointer vector
    std::vector<const geo2d::Line<float>*> pcaptr_v;
    std::vector<int> cluster2pca;
    pcaptr_v.reserve(clusters.size());
    cluster2pca.resize(clusters.size());			 
    auto const& pca_lines_v_v = pca_data._ctor_lines_v_v_v[meta.plane()];
    auto const& pca_aid_v_v = pca_data._atomic_id_v_v_v[meta.plane()];
    for(size_t i = 0; i < pca_lines_v_v.size(); i++) {
      for(size_t j = 0; j < pca_lines_v_v[i].size(); j++) {
	auto const& atomic_id = pca_aid_v_v[i][j];
	if(atomic_id >= clusters.size()) throw larbys("Atomic ID out of bound!");
	cluster2pca.at(atomic_id) = pcaptr_v.size();
	pcaptr_v.push_back(&(pca_lines_v_v[i][j]));
      }
    }

    // Define a bounding box in which we will work
    geo2d::Vector<float> box_tl;
    geo2d::Vector<float> box_br;
    for(size_t c_idx=0; c_idx < clusters.size(); ++c_idx) {
      auto const rect = cv::boundingRect( cv::Mat(clusters[c_idx]._contour) );
      auto const tl = rect.tl();
      auto const br = rect.br();
      if(!c_idx) {
	box_tl.x = tl.x; box_tl.y = tl.y;
	box_br.x = br.x; box_br.y = br.y;
	continue;
      }
      if(tl.x < box_tl.x) box_tl.x = tl.x;
      if(tl.y > box_tl.y) box_tl.y = tl.y;
      if(br.x > box_br.x) box_br.x = br.x;
      if(br.y < box_br.y) box_br.y = br.y;
    }
    
    auto const& box_origin = box_tl;
    const float box_width  = box_br.x - box_tl.x;
    const float box_height = box_tl.y - box_br.y;
    auto const  bbox = cv::Rect(box_origin.x, box_origin.y, box_width, box_height);

    // Crop the image
    auto const small_img = cv::Mat(img,bbox);
    //cv::threshold(small_img,small_img,_pi_threshold,1000,3);

    // List points that we care
    auto& pts = data._pts_vv[meta.plane()];
    pts.clear();
    findNonZero(small_img, pts);
    // Correct point's coordinate
    for(auto& pt : pts) {
      pt.x += box_origin.x;
      pt.y += box_origin.y;
    }

    // create pt=>cluster map
    auto& pt2cluster = data._pt2cluster_vv[meta.plane()];
    pt2cluster.resize(pts.size());
    for(auto& v : pt2cluster) v = kINVALID_SIZE;
    
    for (unsigned pt_idx = 0; pt_idx < pts.size(); ++pt_idx) {
      auto const& pt = pts[pt_idx];
      for(size_t c_idx = 0; c_idx < clusters.size(); ++c_idx) {
	auto const& ctor = clusters[c_idx]._contour;
	if( pointPolygonTest(ctor, pt, false) <0 ) continue;
	pt2cluster[pt_idx] = c_idx;
	break;
      }
    }

    // compute a point projection on each corresponding pca
    // record min/max bounds later for binning
    std::pair<float,float> def_bound;
    def_bound.first  = kINVALID_FLOAT;
    def_bound.second = kINVALID_FLOAT;
    auto& pt2dist = data._pt2dist_vv[meta.plane()];
    auto& bounds_v = data._bounds_vv[meta.plane()];
    pt2dist.resize(pt2cluster.size());
    for(auto& v : pt2dist) v = kINVALID_FLOAT;
    bounds_v.resize(clusters.size());
    for(auto& v : bounds_v) v = def_bound;

    geo2d::Vector<float> pt0, pt1, pt2;
    float dist;
    for(size_t pt_idx=0; pt_idx < pts.size(); ++pt_idx) {
      auto const& c_idx = pt2cluster[pt_idx];
      if(c_idx == kINVALID_SIZE) continue;
      if(cluster2pca.at(c_idx) < 0) throw larbys("PCA not found");
      auto const& pcaptr = pcaptr_v.at(cluster2pca[c_idx]);

      pt0.x = pts[pt_idx].x; pt0.y = pts[pt_idx].y;
      geo2d::ClosestPoint<float>(*pcaptr, pt0, pt1, pt2);
      pt1 -= pcaptr->pt;

      if(pcaptr->dir.x) dist = pt1.x / pcaptr->dir.x;
      else dist = pt1.y / pcaptr->dir.y;

      pt2dist[pt_idx] = dist;

      auto& bound = bounds_v.at(c_idx);
      if(bound.first == kINVALID_FLOAT)
	{ bound.first = bound.second = dist; }
      else {
	if(bound.first  > dist) bound.first  = dist;
	if(bound.second < dist) bound.second = dist;
      }
    }

    // Now compute binned distribution
    auto& dqdx_vv = data._dqdx_vv;
    dqdx_vv.resize(clusters.size());
    for(size_t c_idx = 0; c_idx < dqdx_vv.size(); ++c_idx) {
      auto& dqdx_v = dqdx_vv.at(c_idx);
      auto const& bounds = bounds_v.at(c_idx);
      if(bounds.first == bounds.second)
	continue;
      size_t num_dqdx = (bounds.second - bounds.first) / _dx_resolution + 1;
      LAROCV_DEBUG() << "Plane " << meta.plane() << " Cluster " << c_idx
		     << " got bounds " << bounds.first << " => " << bounds.second
		     << " ... # bins " << num_dqdx << std::endl;
      dqdx_v.resize(num_dqdx);
      for(auto& d : dqdx_v) d = 0.;
    }
    size_t num_invalid_pts = 0;
    for(size_t pt_idx = 0; pt_idx < pts.size(); ++pt_idx) {
      auto const& c_idx  = pt2cluster.at(pt_idx);
      if(c_idx == kINVALID_SIZE) {
	num_invalid_pts++;
	continue;
      }
      auto const& pt     = pts.at(pt_idx);
      auto const& bounds = bounds_v.at(c_idx);
      auto const& dist   = pt2dist.at(pt_idx);
      size_t dqdx_idx = (dist - bounds.first) / _dx_resolution;
      LAROCV_DEBUG() << "Point " << pt_idx << " @ (" << pt.x << "," << pt.y << ") @ " << dist
		     << " belongs to cluster " << c_idx << std::endl;
      dqdx_vv.at(c_idx).at(dqdx_idx) += (float)(img.at<unsigned char>(pt.y,pt.x));
    }
    LAROCV_DEBUG() << "Number of points not processed: " << num_invalid_pts << "/" << pts.size() << std::endl;
  }

  void dQdXProfiler::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {}
  
  
}
#endif
