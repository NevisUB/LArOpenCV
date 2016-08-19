#ifndef __FILTERROI_CXX__
#define __FILTERROI_CXX__

#include "FilterROI.h"

namespace larocv {

  void FilterROI::_Configure_(const ::fcllite::PSet &pset)
  {

  }
  
  Cluster2DArray_t FilterROI::_Process_(const larocv::Cluster2DArray_t& clusters,
						const ::cv::Mat& img,
						larocv::ImageMeta& meta, larocv::ROI& roi)
  {
    return clusters;
  }
  
}
#endif
