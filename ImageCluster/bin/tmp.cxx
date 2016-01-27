#ifndef __CLASSNAME_CXX__
#define __CLASSNAME_CXX__

#include "ClassName.h"

namespace larcv{

  void ClassName::_Configure_(const ::fcllite::PSet &pset)
  {}

  ContourArray_t ClassName::_Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  { return ContourArray_t(); }

}
#endif
