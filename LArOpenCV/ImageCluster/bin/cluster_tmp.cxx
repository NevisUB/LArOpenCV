#ifndef __CLASSNAME_CXX__
#define __CLASSNAME_CXX__

#include "ClassName.h"

namespace larocv{

  void ClassName::_Configure_(const ::fcllite::PSet &pset)
  {}

  Cluster2DArray_t ClassName::_Process_(const larocv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larocv::ImageMeta& meta,
					larocv::ROI& roi)
  { return Cluster2DArray_t(); }

}
#endif
