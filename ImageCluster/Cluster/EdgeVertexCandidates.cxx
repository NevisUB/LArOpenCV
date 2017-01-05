#ifndef __EDGEVERTEXCANDIDATES_CXX__
#define __EDGEVERTEXCANDIDATES_CXX__

#include "EdgeVertexCandidates.h"
#include "Core/Line.h"
#include "AlgoData/dQdXProfilerData.h"
#include "AlgoData/LinearTrackClusterData.h"
#include "GenUtils.h"

namespace larocv {

  /// Global larocv::EdgeVertexCandidatesFactory to register AlgoFactory
  static EdgeVertexCandidatesFactory __global_EdgeVertexCandidatesFactory__;

  void EdgeVertexCandidates::_Configure_(const ::fcllite::PSet &pset)
  {
    LAROCV_DEBUG() << "processing" << std::endl;

    auto const lintrack_algo_name = pset.get<std::string>("LinearTrackAlgoName","");
    
    if(lintrack_algo_name.empty()) {
      LAROCV_CRITICAL() << "LinearTrackAlgoName not specified" << std::endl; 
      throw larbys();
    } else {
      _lintrack_algo_id = this->ID(lintrack_algo_name);
      if(_lintrack_algo_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Invalid LinearTrack algorithm name given: "
			  << lintrack_algo_name
			  << std::endl;
	throw larbys();
      }
      LAROCV_NORMAL() << "EdgeVertexCandidates " << this->Name()
		      << " will use lintrack algorithm " << lintrack_algo_name
		      << std::endl;
      
    }
    
    auto const dqdx_algo_name = pset.get<std::string>("dQdXAlgoName","");

    if(dqdx_algo_name.empty()) {
      LAROCV_CRITICAL() << "dQdXAlgoName not specified" << std::endl; 
      throw larbys();
    } else {
      _dqdx_algo_id = this->ID(dqdx_algo_name);
      if(_dqdx_algo_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Invalid dqdx algorithm name given: "
			  << dqdx_algo_name
			  << std::endl;
	throw larbys();
      }
      LAROCV_NORMAL() << "EdgeVertexCandidates " << this->Name()
		      << " will use dQdX algorithm " << dqdx_algo_name
		      << std::endl;
      
    }
    
    
  }


  void EdgeVertexCandidates::_Process_(const larocv::Cluster2DArray_t& clusters,
				const ::cv::Mat& img,
				larocv::ImageMeta& meta,
				larocv::ROI& roi)
  {
    LAROCV_DEBUG() << "processing" << std::endl;
  }
  
  bool EdgeVertexCandidates::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    LAROCV_DEBUG() << "processing" << std::endl;

    const auto& dqdx_data   = AlgoData<data::dQdXProfilerData>(_dqdx_algo_id);
    const auto& lintrk_data = AlgoData<data::LinearTrackArray>(_lintrack_algo_id);

    LAROCV_DEBUG() << "LinearTrackClusters: " << lintrk_data.get_clusters().size() << std::endl;
    LAROCV_DEBUG() << "VertexTrackClusters: " << dqdx_data.get_vertex_clusters().size() << std::endl;
    
    // for each linear track cluster
    for(const auto & trk_cluster : lintrk_data.get_clusters()) {
      
      const auto& vtx_dqdx = dqdx_data.get_vertex_cluster(trk_cluster.id());

      LAROCV_DEBUG() << "Got vertex dqdx from track cluster id  " << trk_cluster.id() << std::endl;
      
      for(uint plane_id=0; plane_id < 3; ++plane_id) {

	LAROCV_DEBUG() << "Plane id " << plane_id << std::endl;
	  
	for (const auto& part_dqdx : vtx_dqdx.get_cluster(plane_id)) {
	  const auto& dqdx_v = part_dqdx.dqdx();
	  LAROCV_DEBUG() << "Got dqdx length " << dqdx_v.size() << std::endl;

	  //calculate the baseline over all atomics
	  size_t pre = 2;
	  size_t post= 2;
	  auto rmean_v = RollingMean(dqdx_v,pre,post);
	  auto rsigma_v= RollingSigma(dqdx_v,pre,post);

	  auto base_rmean  = BinnedMaxOccurrence(rmean_v,100);
	  auto base_rsigma = BinnedMaxOccurrence(rsigma_v,100);

	  LAROCV_DEBUG() << "Mean of dqdx : " << Mean(dqdx_v) << std::endl;
	  LAROCV_DEBUG() << "Sig  of dqdx : " << Sigma(dqdx_v) << std::endl;
	  
	  LAROCV_DEBUG() << "Binned Mean of dqdx : " << base_rmean << std::endl;
	  LAROCV_DEBUG() << "Binned Sig  of dqdx : " << base_rsigma << std::endl;

	  //Get the global max of rolling mean, hope there is no duplicates
	  auto max_itr = std::max_element(rmean_v.begin(),rmean_v.end());
	  size_t max_index = std::distance(rmean_v.begin(),max_itr);
	  auto   max_value = *max_itr;
	  LAROCV_DEBUG() << "Found global max of rolled mean at position " << max_index << " with value " << max_value << std::endl;

	  // What if our spectrum is noisy?
	  float sigma_base = 5.0;
	  if ( (base_rmean / base_rsigma)  < sigma_base ) 
	    LAROCV_INFO() << "Found a noisy spectrum! Careful!" << std::endl;
	  
	  
	  // NOTE: Handle case global max is smaller than f0*sigma

	  float sigma_thresh = 5.0;
	  float rsigma_bound = sigma_thresh * base_rsigma;
	  
	  //Scan left and right, if we hit the edge (+/- pre/post) OR or go below sigma_thresh sigma, stop

	  // counting index, left index, right index,
	  size_t idx  = kINVALID_SIZE;
	  size_t ridx = kINVALID_SIZE;
	  size_t lidx = kINVALID_SIZE;

	  // left edge hit, right edge hit
	  bool ledge = false;
	  bool redge = false;
	  
	  // go right
	  LAROCV_DEBUG() << "Checking right edge..." << std::endl;
	  for(idx = max_index+1; idx < rmean_v.size() - post; ++idx) {
	    LAROCV_DEBUG() << "( " << rmean_v.at(idx) << "," << rsigma_bound << ")" << std::endl;
	    if ( rmean_v.at(idx) < base_rmean + rsigma_bound ) {
	      ridx = idx;
	      LAROCV_DEBUG() << "ridx set" << std::endl;
	      break;
	    }
	  }

	  if (ridx == kINVALID_SIZE) redge = true;

	  // go left
	  LAROCV_DEBUG() << "Checking left edge..." << std::endl;
	  for(idx = max_index-1; idx > pre; --idx) {
	    LAROCV_DEBUG() << "( " << rmean_v.at(idx) << "," << rsigma_bound << ")" << std::endl;
	    if ( rmean_v.at(idx) < base_rmean + rsigma_bound ) {
	      lidx = idx;
	      LAROCV_DEBUG() << "lidx set" << std::endl;
	      break;
	    }
	  }
	  
	  if (lidx == kINVALID_SIZE) ledge = true;

	  // both edges found
	  if ( ledge && redge ) {
	    LAROCV_CRITICAL() << "Left and right edges found in scan" << std::endl;
	    throw larbys();
	  }

	  // both edges NOT found
	  if ( !ledge && !redge) {
	    LAROCV_INFO() << "Neither left or right edges found in scan" << std::endl;

	    //Assigned the edge closest to ridx or lidx;
	    size_t d_left  = lidx - pre;
	    size_t d_right = rmean_v.size() - post - ridx;

	    if (d_left > d_right) ledge = true;
	    if (d_left < d_right) redge = true;

	    if ( !ledge && !redge ) throw larbys("Neither left or right edge could be determined, they are the same num?");
	    
	  }
	  
	  LAROCV_DEBUG() << "ledge " << ledge << "... redge " << redge << std::endl;
	  LAROCV_DEBUG() << "lidx  " << lidx  << "... ridx  " << ridx << std::endl;

	  // Determine where along PCA the candidate vertex location
	  
	  
	  
	}
      }
    }
    return true;
  }
}
  

#endif
// std::cout << "[";
// for(const auto& s : rmean_v) std::cout << s << ",";
// std::cout << "]" << std::endl;
// std::cout << std::endl;

