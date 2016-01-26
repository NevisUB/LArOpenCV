#ifndef __IMAGECLUSTERBASE_CXX__
#define __IMAGECLUSTERBASE_CXX__

#include "ImageClusterBase.h"

namespace larcv {

  ImageClusterBase::ImageClusterBase(const std::string name)
    : _name(name)
    , _proc_time(0.)
    , _proc_count(0.)
  {}

  ContourArray_t ImageClusterBase::Process(const larcv::ContourArray_t& clusters,
					   const ::cv::Mat& img,
					   larcv::ImageMeta& meta)
  {
    _watch.Start();
    auto result = this->_Process_(clusters,img,meta);
    _proc_time += _watch.WallTime();
    ++_proc_count;
    return result;
  }

}

#endif
