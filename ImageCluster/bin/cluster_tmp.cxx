#ifndef __CLASSNAME_CXX__
#define __CLASSNAME_CXX__

#include "ClassName.h"

namespace larcv{

  void ClassName::_Configure_(const ::fcllite::PSet &pset)
  {}

  Cluster2DArray_t ClassName::_Process_(const larcv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larcv::ImageMeta& meta)
  { return Cluster2DArray_t(); }

}
#endif
