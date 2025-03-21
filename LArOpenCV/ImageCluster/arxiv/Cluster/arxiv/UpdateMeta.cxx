#ifndef __UPDATEMETA_CXX__
#define __UPDATEMETA_CXX__

#include "UpdateMeta.h"

namespace larocv{

  void UpdateMeta::_Configure_(const ::fcllite::PSet &pset)
  { }

  Cluster2DArray_t UpdateMeta::_Process_(const larocv::Cluster2DArray_t& clusters,
					 const ::cv::Mat& img,
					 larocv::ImageMeta& meta, larocv::ROI& roi)
  {


    // This algorithm resets the hit locations considered for merging to those hits 
    // left in current clusters (eg, clusters with only 2/3 hits will have been removed 
    // by SimpleCuts; their hits will no longer make it into the merging pool)
    meta.clear_locations();

    for(auto& cluster : clusters) {
      //std::cout<<"*******hits + locs : "<<cluster._insideHits.size()<<", "<<meta.get_locations().size()<<std::endl ;
      for(auto const & i : cluster._insideHits )
        meta.add_location(i);
    }    

    //std::cout<<"Meta locations : "<<meta.get_locations().size() <<std::endl ;
    
    return clusters;
    
  }

}
#endif
