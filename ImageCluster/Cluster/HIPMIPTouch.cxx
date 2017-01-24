#ifndef __HIPMIPTOUCH_CXX__
#define __HIPMIPTOUCH_CXX__

#include "HIPCluster.h"
#include "HIPMIPTouch.h"

namespace larocv {

  /// Global larocv::HIPMIPTouchFactory to register AlgoFactory
  static HIPMIPTouchFactory __global_HIPMIPTouchFactory__;

  void HIPMIPTouch::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const hip_cluster_algo_name = pset.get<std::string>("HIPClusterAlgo","hipctor");
    _hip_cluster_algo_id = this->ID(hip_cluster_algo_name);

    Register(new data::HIPMIPTouchData);
  }


  void HIPMIPTouch::_Process_(const larocv::Cluster2DArray_t& clusters,
			      const ::cv::Mat& img,
			      larocv::ImageMeta& meta,
			      larocv::ROI& roi)
  {

    const auto& hipcluster_data = AlgoData<data::HIPClusterData>(_hip_cluster_algo_id,0)._plane_data_v[meta.plane()];
    auto& data                  = AlgoData<data::HIPMIPTouchData>(0);
    auto& hip_mip_touch_pts_v   = data._hip_mip_touch_pts_v_v[meta.plane()];
    
    //get the mip/hip indices
    auto& mip_ass_v = hipcluster_data.get_mip_indicies();
    auto& hip_ass_v = hipcluster_data.get_hip_indicies();

    if (mip_ass_v.size() + hip_ass_v.size() != clusters.size()) {
      LAROCV_ERROR() << "HIP/MIP touch algo called after contour breaking" << std::endl;
      throw larbys();
    }
    
    for(auto hip_cidx : hip_ass_v) {
      
      auto& hip_ctor = clusters.at(hip_cidx)._contour;
      float min_d=1.e4;
      const cv::Point_<int>* close_pt = nullptr;
      
      //for each hip point
      for(auto& hip_pt : hip_ctor) {
      
	for(auto mip_cidx : mip_ass_v) {
	  auto& mip_ctor = clusters.at(mip_cidx)._contour;
	  for(auto& mip_pt : mip_ctor) {
	    auto d = geo2d::dist(geo2d::Vector<float>(mip_pt),geo2d::Vector<float>(hip_pt));
	    if (d < min_d) {
	      min_d=d;
	      close_pt = &hip_pt;
	    }//this one is smaller
	  }//end loop over mip pts
	}//end loop over all mips

	LAROCV_DEBUG() << "Found a HIP MIP close point @ " << *close_pt << std::endl;
	hip_mip_touch_pts_v.emplace_back(close_pt->x,close_pt->y); //ensures type conversion
	
      }//end loop over hips
      
    }

    
  }
    bool HIPMIPTouch::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    return true;
  }
  
  
}
#endif
