#ifndef __DQDXPROFILER_CXX__
#define __DQDXPROFILER_CXX__

#include "dQdXProfiler.h"
#include "PCACandidates.h"
#include "DefectCluster.h"
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
    _dx_resolution = 1.;
  }
  
  void dQdXProfiler::_Process_(const larocv::Cluster2DArray_t& clusters,
			       const ::cv::Mat& img,
			       larocv::ImageMeta& meta,
			       larocv::ROI& roi)
  {
    cv::Mat thresh_img = img;
    //::cv::threshold(img, thresh_img, 10,255,CV_THRESH_BINARY);

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
    geo2d::Vector<float> box_tl(-1,-1);
    geo2d::Vector<float> box_br(-1,-1);
    for(size_t c_idx=0; c_idx < clusters.size(); ++c_idx) {
      LAROCV_DEBUG() << "Current box1:" << box_tl << " => " << box_br << std::endl;
      if(clusters[c_idx]._contour.size()<3) continue;
      // for(auto const& pt : clusters[c_idx]._contour)
      // 	LAROCV_DEBUG() << "    Pt: " << pt << std::endl;
      auto const rect = cv::boundingRect( cv::Mat(clusters[c_idx]._contour) );
      LAROCV_DEBUG() << "Cluster ID " << c_idx
		     << " points=" << clusters[c_idx]._contour.size() << " ... Box: " << rect << std::endl;
      auto const tl = rect.tl();
      auto const br = rect.br();
      if(box_tl.x < 0) {
	box_tl.x = tl.x; box_tl.y = tl.y;
	box_br.x = br.x; box_br.y = br.y;
	continue;
      }
      if(tl.x < box_tl.x) box_tl.x = tl.x;
      if(tl.y < box_tl.y) box_tl.y = tl.y;
      if(br.x > box_br.x) box_br.x = br.x;
      if(br.y > box_br.y) box_br.y = br.y;

      LAROCV_DEBUG() << "Current box2:" << box_tl << " => " << box_br << std::endl;
    }

    auto const& box_origin = box_tl;
    const float box_width  = box_br.x - box_tl.x;
    const float box_height = box_br.y - box_tl.y;
    LAROCV_DEBUG() << "Ground BBox TL: " << box_tl << " width: " << box_width << " height: " << box_height << std::endl;
    auto const bbox = cv::Rect(box_origin.x, box_origin.y, box_width, box_height);

    // Crop the image
    auto const small_img = cv::Mat(thresh_img,bbox);

    //cv::threshold(small_img,small_img,_pi_threshold,1000,3);

    // List points that we care
    auto& pts = data._pts_vv[meta.plane()];
    pts.clear();
    cv::findNonZero(small_img, pts);
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
	if( cv::pointPolygonTest(ctor, pt, false) < 0 ) continue;
	pt2cluster[pt_idx] = c_idx;
	break;
      }
    }

    //some points not associated to a cluster, lets "attach" them to the nearest one, with distance cut
    for (unsigned pt_idx = 0; pt_idx < pts.size(); ++pt_idx) {
      auto const& pt = pts[pt_idx];
      if (pt2cluster[pt_idx] != kINVALID_SIZE) continue;

      int   min_idx=-1;
      float min_d  =9.e6;
      
      for(size_t c_idx = 0; c_idx < clusters.size(); ++c_idx) {
	auto const& ctor = clusters[c_idx]._contour;
	for(const auto& ctor_pt : ctor) {
	  float d=std::sqrt(std::pow(pt.x-ctor_pt.x,2)+std::pow(pt.y-ctor_pt.y,2));
	  if (d < min_d) {
	    min_d   = d;
	    min_idx = c_idx;
	  }
	}
      }

      if ( min_d>5 )   min_idx=-1;
      if (min_idx==-1) continue;

      pt2cluster[pt_idx] = min_idx;
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
      else              dist = pt1.y / pcaptr->dir.y;

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
      if(bounds.first == bounds.second)	continue;
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
      // LAROCV_DEBUG() << "Point " << pt_idx << " @ (" << pt.x << "," << pt.y << ") @ " << dist
      // 	        << " belongs to cluster " << c_idx << std::endl;
      dqdx_vv.at(c_idx).at(dqdx_idx) += (float)(thresh_img.at<unsigned char>(pt.y,pt.x));
    }
    LAROCV_DEBUG() << "Number of points not processed: " << num_invalid_pts << "/" << pts.size() << std::endl;

    // order the contours
    const auto& n_clusters = defect_data._n_original_clusters_v[meta.plane()];
    const auto& atomic_ctor_ass_v = defect_data._atomic_ctor_ass_v_v[meta.plane()];

    std::vector<std::vector<uint> > dfect_cidx_v_v;
    std::vector<std::vector<uint> > odfect_cidx_v_v;
    
    dfect_cidx_v_v.resize(n_clusters);
    odfect_cidx_v_v.resize(n_clusters);


    LAROCV_DEBUG() << " N : " << n_clusters << " atomic clusters incoming " << std::endl;
    LAROCV_DEBUG() << " Number of defect clusters should be " << atomic_ctor_ass_v.size() << std::endl;

    //for each of the original atomic cluster	  
    for(unsigned atomic_cidx=0; atomic_cidx<n_clusters; ++atomic_cidx) {

      //get this set of indicies
      auto& dfect_cidx_v  = dfect_cidx_v_v[atomic_cidx];

      //get this ordered (to be determined...) set of indicies
      auto& odfect_cidx_v = odfect_cidx_v_v[atomic_cidx];
      
      odfect_cidx_v.clear();

      //for each of the associated broken=>atomic association
      for(unsigned jj=0;jj<atomic_ctor_ass_v.size();++jj) {
	const auto& atomic_ctor_ass=atomic_ctor_ass_v[jj];

	//if this associated, broken cluster is not from atomic cluster atomic_cidx, move on
	if ( atomic_ctor_ass != atomic_cidx ) continue;

	//else this broken cluster came from same atomic
	dfect_cidx_v.emplace_back(jj);
      }

      LAROCV_DEBUG() << " N : " << dfect_cidx_v.size() << " associated defect clusters to atomic " << atomic_cidx << std::endl;
      
      //lets get the left most contour
      uint min_idx=-1;
      int min_x=9e6;

      //for each broken cluster (from the same atomic!)
      for(unsigned kk=0;kk<dfect_cidx_v.size();++kk)  {
	auto& ctor=clusters[ dfect_cidx_v[kk] ]._contour;
	for(const auto& pt: ctor) {
	  if(pt.x<min_x) {
	    min_x = pt.x;
	    min_idx=dfect_cidx_v[kk];
	  }
	}
      }

      LAROCV_DEBUG() << " Putting min_idx : " << min_idx << " into odfect_cidx_v\n";
      
      //put in the left most index
      odfect_cidx_v.push_back(min_idx);

      //until the ordered index vector is filled
      while(odfect_cidx_v.size() != dfect_cidx_v.size()) {

	//get the last index in ordered vector
	auto  curr_cidx  = odfect_cidx_v.back();
	auto& curr_ctor  = clusters[curr_cidx]._contour;
	
	int min_idx=-1;
	int min_d=9e6;
	  
	//for each of the broken clusters from the same atomic
	for(unsigned kk=0;kk<dfect_cidx_v.size();++kk)  {

	  //if this broken cluster aleady ordered, move on
	  if ( std::find(odfect_cidx_v.begin(), odfect_cidx_v.end(), dfect_cidx_v[kk]) != odfect_cidx_v.end() )
	    continue;

	  auto& part_ctor=clusters[dfect_cidx_v[kk]]._contour;
	  
	  //else, loop pairwire between current contour and this one, and determine the min distance
	  for(const auto& curr_pt : curr_ctor) {
	    for(const auto& part_pt: part_ctor) {
	      float d=std::sqrt(std::pow(part_pt.x-curr_pt.x,2)+std::pow(part_pt.y-curr_pt.y,2));
	      if (d < min_d) {
		min_d=d;
		min_idx=dfect_cidx_v[kk];
	      }
	    }
	  }
	}
	
	//min_idx now filled with index which contains the closest contour
	if (min_idx==-1){
	  LAROCV_CRITICAL() << "curr_cidx: " << curr_cidx << " min_d : " << min_d << " min_idx : " << min_idx << std::endl;
	  throw larbys("Could not find minimum index for this contour\n");
	}
	
	//put it into ordering scheme
	odfect_cidx_v.push_back(min_idx);
      }
    }

    auto& o_dqdx_vv=data._o_dqdx_vvv[meta.plane()];
    o_dqdx_vv.resize(n_clusters);
    for(unsigned atomic_cidx=0; atomic_cidx<n_clusters; ++atomic_cidx) {
      auto& o_dqdx_v=o_dqdx_vv[atomic_cidx];
      for (const auto& odfect_cidx : odfect_cidx_v_v[atomic_cidx]) {
	for(const auto& dqdx : dqdx_vv.at(odfect_cidx)) {
	  o_dqdx_v.push_back(dqdx);
	}
      }
    }
    
    
  }

  bool dQdXProfiler::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  { return true; }
  
  
}
#endif
