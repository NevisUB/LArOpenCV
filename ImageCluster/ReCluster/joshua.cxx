#ifndef __JOSHUA_CXX__
#define __JOSHUA_CXX__

#include "joshua.h"

namespace larocv{

  
  void joshua::_Configure_(const ::fcllite::PSet &pset)
  {
  }
  
  Cluster2DArray_t joshua::_Process_(const larocv::Cluster2DArray_t& clusters,
					      const ::cv::Mat& img,
					      larocv::ImageMeta& meta)
  {
    return Cluster2DArray_t();
  }


}
#endif
