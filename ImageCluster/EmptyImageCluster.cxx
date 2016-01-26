#ifndef __EMPTYIMAGECLUSTER_CXX__
#define __EMPTYIMAGECLUSTER_CXX__

#include "EmptyImageCluster.h"
#include "Core/larbys.h"
#include <opencv2/imgproc/imgproc.hpp>
namespace larcv{

  void EmptyImageCluster::Configure(const ::fcllite::PSet &pset)
  {}

  ContourArray_t EmptyImageCluster::_Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  { return ContourArray_t(); }

}
#endif
