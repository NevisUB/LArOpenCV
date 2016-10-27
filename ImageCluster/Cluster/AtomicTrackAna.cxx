#ifndef __ATOMICTRACKANA_CXX__
#define __ATOMICTRACKANA_CXX__

#include "AtomicTrackAna.h"
#include "PCACandidatesData.h"
#include "DefectClusterData.h" 
#include "Refine2DVertex.h"


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
    auto const ref_algo_name = pset.get<std::string>("Refine2DVertex");
    _ref_algo_id = this->ID(ref_algo_name);
    

    
  }

  void AtomicTrackAna::_Process_(const larocv::Cluster2DArray_t& clusters,//? clusters from ???
				 const ::cv::Mat& img,
				 larocv::ImageMeta& meta,
				 larocv::ROI& roi)
  {
    auto& data = AlgoData<larocv::AtomicTrackAnaData>();
    
    if(meta.plane()==0) {
      data._atomic_order_ctor_v_v.clear();
      data._atomic_order_ctor_v_v.resize(3);
      data._atomic_kink_v.clear();
      data._atomic_kink_v.resize(3);
    }

    const auto& defectcluster_data  = this->AlgoData<larocv::DefectClusterData>(_def_algo_id); //Read defect points from previous calculations
    auto const& pcacandidates_data  = this->AlgoData<larocv::PCACandidatesData>(_pca_algo_id); //Read PCA results from previous calculations
    auto const& refine2dvertex_data = this->AlgoData<larocv::Refine2DVertexData>(_ref_algo_id);//Read vertex info from Refine2D
    
    
    
    LAROCV_DEBUG() << "On plane : " << meta.plane() << std::endl;
    LAROCV_DEBUG()<<"4_ref_algo_id is "<<_ref_algo_id<<std::endl;
    LAROCV_DEBUG()<<"6_def_algo_id is "<<_def_algo_id<<std::endl;
    LAROCV_DEBUG()<<"7_pca_algo_id is "<<_pca_algo_id<<std::endl;
    LAROCV_DEBUG()<<"this->ID() is    "<<this->ID()  <<std::endl;
    
    //atomic_defect_v_v_v
    //3planes//clusters//defects
    auto const& atomic_defects_v_v  = defectcluster_data._atomic_defect_v_v_v[meta.plane()];//Get defects per track cluster after vertex clustering
    auto const& atomic_ctr_v = defectcluster_data._atomic_ctor_v_v[meta.plane()];
    auto const& atomic_ass_v = defectcluster_data._atomic_ctor_ass_v_v[meta.plane()];    
    auto const& n_track_clusters = defectcluster_data._n_original_clusters_v[meta.plane()];
    
    auto& ctor_order_v  = data._atomic_order_ctor_v_v[meta.plane()];
    auto& kink_point_v  = data._atomic_kink_v[meta.plane()];

    //per plane //per original cluster // atomics
        
    auto const& ref_vtx = refine2dvertex_data._cand_vtx_v[meta.plane()];//Get vertex per plane
    LAROCV_DEBUG() << "For now there is one vertex per plane, x is  : " << ref_vtx.x << " y is " << ref_vtx.y<< std::endl;
    LAROCV_DEBUG() << "size of atomics "<<atomic_defects_v_v.size()<<std::endl;//
    LAROCV_DEBUG() << "size of clusters(after defects)  "<<clusters.size()<<std::endl;
    
    //    for (unsigned ic=0; ic<clusters.size(); ++ic){

    ctor_order_v.resize(n_track_clusters);
    kink_point_v.resize(n_track_clusters);
    
    //Loop over all track clusters after vertex clutersing
    for(unsigned int i=0; i< n_track_clusters; ++i) {
      kink_point_v[i].clear();
      ctor_order_v[i].clear();
	    
      auto& atomic_defects_v = atomic_defects_v_v[i];
      
      //vector of atomics idx for this track cluster
      std::vector<size_t> atomic_index_v;
      atomic_index_v.clear();

      //Store associated atmoic
      for(size_t j=0;j<atomic_ass_v.size();++j){ 
	if (atomic_ass_v[j] != i) continue;
	atomic_index_v.push_back(j);
      }
      
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
	  ctor_order_v[i].push_back(clusters[atomic_index_v_pair[k].first]._contour);
	  atomic_index_v_pair[k].second = false;
	  //LAROCV_DEBUG()<<"atomic size on this one track contour is  "<<clusters[atomic_index_v_pair[k].first]._contour.size()<<std::endl;
	  LAROCV_DEBUG()<<"This atomic should be blind in next calculatiion "<<k<<std::endl;
	  LAROCV_DEBUG()<<"bool value from "<<k<<"is "<<atomic_index_v_pair[k].second<<std::endl;
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
	//ctor_order_v[i].push_back(clusters[order_idx]._contour);
	ctor_order_v[i].push_back(clusters[atomic_index_v_pair[order_idx].first]._contour);
	kink_point_v[i].push_back(vtx);
	LAROCV_DEBUG()<<"size of ctor_order_v is  "<<ctor_order_v[i].size()<<std::endl;
	atomic_index_v_pair[order_idx].second=false;
	dist_max=0;
	dist_min=99999;
      }
      
      //LAROCV_DEBUG()<<"size of atmoic_defects "<<atomic_defects_v.size()<<std::endl;
      
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
