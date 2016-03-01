#ifndef __PCAOVERALL_CXX__
#define __PCAOVERALL_CXX__

#include "PCAOverall.h"

namespace larcv{

  void PCAOverall::_Configure_(const ::fcllite::PSet &pset)
  {}

  Cluster2DArray_t PCAOverall::_Process_(const larcv::Cluster2DArray_t& clusters,
					 const ::cv::Mat& img,
					 larcv::ImageMeta& meta)
  {
    
    for(auto& cluster : clusters) {
      
      
    }
  }
}
#endif
