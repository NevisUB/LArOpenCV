#ifndef __EMPTYIMAGECLUSTER_CXX__
#define __EMPTYIMAGECLUSTER_CXX__

#include "EmptyImageCluster.h"
namespace larcv{

  void EmptyImageCluster::_Configure_(const ::fcllite::PSet &pset)
  {}

  ContourArray_t EmptyImageCluster::_Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  { return ContourArray_t(); }

}
#endif
