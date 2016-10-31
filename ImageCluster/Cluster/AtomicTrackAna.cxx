#ifndef __ATOMICTRACKANA_CXX__
#define __ATOMICTRACKANA_CXX__

#include "AtomicTrackAna.h"
#include "PCACandidates.h"
#include "DefectCluster.h" 
#include "Refine2DVertex.h"
#include <numeric>

namespace larocv{

  /// Global larocv::AtomicTrackAnaFactory to register ClusterAlgoFactory
  //  10/26, chain order of AtomicTrack is at 8, -1 is pca2, -2 is defec. Many ways to do it, change the unit fcl file 
  static AtomicTrackAnaFactory __global_AtomicTrackAnaFactory__;

  void AtomicTrackAna::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const pca_algo_name = pset.get<std::string>("2ndPCACandidatesName");
    _pca_algo_id = this->ID(pca_algo_name);
    auto const def_algo_name = pset.get<std::string>("2ndDefectClusterName");
    _def_algo_id = this->ID(def_algo_name);
    auto const ref_algo_name = pset.get<std::string>("Refine2DVertexName");
    _ref_algo_id = this->ID(ref_algo_name);
  }

  void AtomicTrackAna::_Process_(const larocv::Cluster2DArray_t& clusters,//
				 const ::cv::Mat& img,
				 larocv::ImageMeta& meta,
				 larocv::ROI& roi)
  {
    auto& data = AlgoData<larocv::AtomicTrackAnaData>();
    
    if(meta.plane()==0) {
      data._atomic_order_ctor_v_v.clear();
      data._atomic_order_ctor_v_v.resize(3);
      data._atomic_kink_vv.clear();
      data._atomic_kink_vv.resize(3);
    }

    auto const& defectcluster_data  = AlgoData<DefectClusterData>(_def_algo_id); //Read defect points from previous calculations
    auto const& pcacandidates_data  = AlgoData<PCACandidatesData>(_pca_algo_id); //Read PCA results from previous calculations
    auto const& refine2dvertex_data = AlgoData<Refine2DVertexData>(_ref_algo_id);//Read vertex info from Refine2D    
    
    LAROCV_DEBUG()<< " On plane : " << meta.plane() << std::endl;
    LAROCV_DEBUG()<< " 4_ref_algo_id is : "<<_ref_algo_id<<std::endl;
    LAROCV_DEBUG()<< " 6_def_algo_id is : "<<_def_algo_id<<std::endl;
    LAROCV_DEBUG()<< " 7_pca_algo_id is : "<<_pca_algo_id<<std::endl;
    LAROCV_DEBUG()<< " this->ID() is    : "<<this->ID()  <<std::endl;
    
    //atomic_defect_v_v_v
    //3planes//clusters//defects
    
    auto const& atomic_defect_pts_vv  = defectcluster_data._atomic_defect_pts_v_v_v[meta.plane()];//Get defects per track cluster after vertex clustering
    //    auto const& atomic_ctr_v        = defectcluster_data._atomic_ctor_v_v[meta.plane()];
    auto const& atomic_ass_vv     = defectcluster_data._atomic_ctor_ass_v_v_v[meta.plane()];    
    auto const& n_track_clusters  = defectcluster_data._n_original_clusters_v[meta.plane()];
    auto const& pca_ctor_lines_v  = pcacandidates_data._ctor_lines_v_v[meta.plane()];
    
    auto& ctor_order_v     = data._atomic_order_ctor_v_v[meta.plane()];
    auto& kink_point_v     = data._atomic_kink_vv[meta.plane()];
    auto& atomic_idx_vv    = data._atomic_idx_per_clu_v_v_v[meta.plane()];
    auto& atomic_spread_vv = data._atomic_spread_vvv[meta.plane()];

    LAROCV_DEBUG()<<"3 size of atomic_idx_per_clu_v_v_v is "<<data._atomic_idx_per_clu_v_v_v.size()<<std::endl;    
    //per plane //per original cluster // atomics
        
    auto const& ref_vtx = refine2dvertex_data._cand_vtx_v[meta.plane()];//Get vertex per plane
    LAROCV_DEBUG() << "For now there is one vertex per plane, x is  : " << ref_vtx.x << " y is " << ref_vtx.y<< std::endl;
    LAROCV_DEBUG() << "size of atomics "<<atomic_defect_pts_vv.size()<<std::endl;//
    LAROCV_DEBUG() << "size of clusters(after defects)  "<<clusters.size()<<std::endl;
    
    ctor_order_v.resize(n_track_clusters);
    kink_point_v.resize(n_track_clusters);
    atomic_idx_vv.resize(n_track_clusters);
    atomic_spread_vv.resize(n_track_clusters);
    
    //Loop over all track clusters after vertex clutersing
    for(unsigned int i=0; i< n_track_clusters; ++i) {
      atomic_idx_vv[i].clear();
      kink_point_v[i].clear();
      ctor_order_v[i].clear();
      
      //vector of atomics idx for this track cluster
      const auto& atomic_index_v = atomic_ass_vv.at(i);
      
      LAROCV_DEBUG()<<"Atomic # on track cluster is "<< atomic_index_v.size()<<std::endl;
      
      if (atomic_index_v.size()==0) LAROCV_DEBUG()<<i <<" th track cluster has no atomic? This does make NO sense!"<<std::endl;

      double dist_max = 0;
      double dist_min = 99999;
      cv::Point point_min;
      cv::Point point_max;
      cv::Point vtx=ref_vtx;

      //This loop finds the atomic cloest to vertex
      std::vector<std::pair<int, bool>> atomic_index_v_pair;
      atomic_index_v_pair.clear();
      for(auto a: atomic_index_v){ 
	std::pair<int, bool> tmp(a, true);
	atomic_index_v_pair.push_back(tmp);
      }
      
      LAROCV_DEBUG()<<"Atomic # on track cluster is from pair"<< atomic_index_v_pair.size()<<std::endl;
      for (auto & poop : atomic_index_v_pair) LAROCV_DEBUG()<<"second in pair vector, should be 1"<<poop.second<<std::endl;
      
      //for (auto& break_ctor : break_ctor_v) {
      int order_idx;
      
      for (unsigned k=0; k<atomic_index_v_pair.size();++k){
	//for (auto const& atomic_index : atomic_index_v) {
	if (atomic_index_v_pair.size()==1) {
	  //LAROCV_DEBUG()<<"Found one track with one contour "<<std::endl;
	  
	  //on the contour found, find the point with max dist
	  /*for (auto const& shit: clusters[atomic_index_v_pair[k].first]._contour){
	    //for (auto const& shit: clusters[order_idx]._contour){
	    double dist_sq = (shit.x-vtx.x)*(shit.x-vtx.x)+ (shit.y-vtx.y)*(shit.y-vtx.y);
	    if (dist_sq > dist_max) {
	      dist_max = dist_sq;
	      point_max.x=shit.x;
	      point_max.y=shit.y;
	    }
	  }
	  
	  vtx=point_max;
	  */
	  ctor_order_v[i].push_back(clusters[atomic_index_v_pair[k].first]._contour);
	  atomic_idx_vv[i].push_back(k);
	  //kink_point_v[i].push_back(vtx);
	  atomic_index_v_pair[k].second = false;
	  //LAROCV_DEBUG()<<"atomic size on this one track contour is  "<<clusters[atomic_index_v_pair[k].first]._contour.size()<<std::endl;
	  LAROCV_DEBUG()<<"This atomic should be blind in next calculatiion "<<k<<std::endl;
	  LAROCV_DEBUG()<<"bool value from "<<k<<"is "<<atomic_index_v_pair[k].second<<std::endl;

	  //Calculate spread on track with 1 ctor
	  auto& pts = data._pts_vv[meta.plane()];
	  
	  pts.clear();
	  
	  cv::findNonZero(img, pts);
	  
	  std::vector<float> dist_v;
	  dist_v.clear();
	  
	  for (auto& pt : pts){
	    auto & ctor = clusters[atomic_index_v_pair[k].first]._contour;
	    if( cv::pointPolygonTest(ctor, pt, false) < 0 ) continue;
	    
	    auto & pca = pca_ctor_lines_v[atomic_index_v_pair[k].first]; //trying to get pca from ordering PCACandidate

	    geo2d::Vector<float>  pt1,pt2;
	    
	    float dist = geo2d::ClosestPoint<float>(pca, pt, pt1, pt2);
	    dist_v.push_back(dist);
	  }
	  if (dist_v.size()>0){
	    double sum    = std::accumulate(dist_v.begin(), dist_v.end(), 0.0);
	    double mean   = sum / dist_v.size();
	    double sq_sum = std::inner_product(dist_v.begin(), dist_v.end(), dist_v.begin(), 0.0);
	    double stdev  = std::sqrt(sq_sum / dist_v.size() - mean * mean);
	    atomic_spread_vv[i].push_back(stdev);
	    LAROCV_DEBUG()<<"mean value is "<<mean<<" rms is "<<stdev<<std::endl;
	    
	  }
	  
	  /////////////////////////////////////////block for track with 1 atomic ends
	  
	  continue;
	}
	//loop all atomic to find min dist point and find its contour
	for (unsigned l=0; l<atomic_index_v_pair.size();++l){
	  
	  LAROCV_DEBUG()<<"second of pair vector on "<< l << " is  "<< atomic_index_v_pair[l].second<<std::endl;
	  
	  if(atomic_index_v_pair[l].second ==false) continue;
	  LAROCV_DEBUG()<<"This atomic are not blind from previous calculatiion "<<l<<std::endl;
	  
	  for (auto const& shit: clusters[atomic_index_v_pair[l].first]._contour){
	    double dist_sq = (shit.x-vtx.x)*(shit.x-vtx.x)+ (shit.y-vtx.y)*(shit.y-vtx.y);
	    if (dist_sq < dist_min) {
	      dist_min = dist_sq;
	      point_min.x=shit.x;
	      point_min.y=shit.y;
	      order_idx = l;
	    }
	  }
	  LAROCV_DEBUG()<<"min dist is "<< dist_min<<std::endl;
	  LAROCV_DEBUG()<<"vertex   point x is "<< vtx.x<<" y is "<<vtx.y<<std::endl;
	  LAROCV_DEBUG()<<"min dist point x is "<< point_min.x<<" y is "<<point_min.y<<std::endl;
	  LAROCV_DEBUG()<<"atomic index is "<< order_idx<<std::endl;
	}
	LAROCV_DEBUG()<<"Found atomic index is "<<order_idx<<std::endl;
	//on the contour found, find the point with max dist
	for (auto const& shit: clusters[atomic_index_v_pair[order_idx].first]._contour){
	//for (auto const& shit: clusters[order_idx]._contour){
	  double dist_sq = (shit.x-vtx.x)*(shit.x-vtx.x)+ (shit.y-vtx.y)*(shit.y-vtx.y);
	  if (dist_sq > dist_max) {
	  dist_max = dist_sq;
	  point_max.x=shit.x;
	  point_max.y=shit.y;
	  }
	}
	
	vtx=point_max;
	LAROCV_DEBUG()<<"size of ctor_order "<<order_idx<<" is  "<<clusters[order_idx]._contour.size()<<std::endl;
	
	//Calculate spread
	auto& pts = data._pts_vv[meta.plane()];
	
	pts.clear();

	cv::findNonZero(img, pts);

	std::vector<float> dist_v;
	dist_v.clear();

	//vic: hey rui i'm commenting this line
	//LAROCV_DEBUG()<<"(should be same as atomic size on this track )size of ctor line is " <<pca_ctor_lines_v[i].size()<<std::endl;
	
	for (auto& pt : pts){
	  auto & ctor = clusters[atomic_index_v_pair[order_idx].first]._contour;
	  if( cv::pointPolygonTest(ctor, pt, false) < 0 ) continue;
	  
	  ///
	  
	  auto const& ctor_pca = clusters[atomic_index_v_pair[order_idx].first]._contour;
	  cv::Mat ctor_pts(ctor_pca.size(), 2, CV_32FC1);
	  for (unsigned i = 0; i < ctor_pts.rows; ++i) {
	    ctor_pts.at<float>(i, 0) = ctor_pca.at(i).x;
	    ctor_pts.at<float>(i, 1) = ctor_pca.at(i).y;
	  }
	  
	  cv::PCA pca_ana(ctor_pts, cv::Mat(), CV_PCA_DATA_AS_ROW,0);
	  
	  geo2d::Line<float> pca_principle(geo2d::Vector<float>(pca_ana.mean.at<float>(0,0),
								pca_ana.mean.at<float>(0,1)),
					 geo2d::Vector<float>(pca_ana.eigenvectors.at<float>(0,0),
							      pca_ana.eigenvectors.at<float>(0,1)));
	  ///
	  //auto & pca = pca_ctor_lines_vv[i][atomic_index_v_pair[order_idx].first];
	  auto & pca = pca_principle;
	  geo2d::Vector<float>  pt1,pt2;
	  
	  float dist = geo2d::ClosestPoint<float>(pca, pt, pt1, pt2);
	  dist_v.push_back(dist);
	}
	if (dist_v.size()>0){
	  double sum    = std::accumulate(dist_v.begin(), dist_v.end(), 0.0);
	  double mean   = sum / dist_v.size();
	  double sq_sum = std::inner_product(dist_v.begin(), dist_v.end(), dist_v.begin(), 0.0);
	  double stdev  = std::sqrt(sq_sum / dist_v.size() - mean * mean);
	  atomic_spread_vv[i].push_back(stdev);
	  LAROCV_DEBUG()<<"mean value is "<<mean<<" rms is "<<stdev<<std::endl;
	  
	}

	/////////////////////////////////////////
	


	ctor_order_v[i].push_back(clusters[atomic_index_v_pair[order_idx].first]._contour);
	
	kink_point_v[i].push_back(vtx);
	atomic_idx_vv[i].push_back(order_idx);
	LAROCV_DEBUG()<<"size of ctor_order_v is  "<<ctor_order_v[i].size()<<std::endl;
	atomic_index_v_pair[order_idx].second=false;
	dist_max=0;
	dist_min=99999;
      }
      
      LAROCV_DEBUG()<<"size of atmoic_defects "<<img.size()<<std::endl;
      
      /*
      for(unsigned int i=0; i< atomic_defects_v.size(); ++i) {
	
	pre_order.resize(atomic_defects_v.size());
	
	LAROCV_DEBUG()<<i <<" th defect x is  "<<atomic_defects_v[i].x << "defect y is "<< atomic_defects_v[i].y<<std::endl;
	
	geo2d::LineSegment<cv::Point> shit(ref_vtx, atomic_defects_v[i]);
	
	double len_sq = 0;
	len_sq= (atomic_defects_v[i].x-ref_vtx.x)*(atomic_defects_v[i].x-ref_vtx.x)+ (atomic_defects_v[i].y-ref_vtx.y)*(atomic_defects_v[i].y-ref_vtx.y);
	
	}*/
      
      
      //LAROCV_DEBUG()<<"size of defects  is "<<atomic_defects_v.size()<<std::endl;
      //if (atomic_defects_v.size() > 10) continue;

    }
    
    //auto const& ref_data = this->AlgoData<larocv::Refine2DVertexData>(_vtx_algo_id);
    
    
    }

  bool AtomicTrackAna::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  { return true; }

}

#endif
