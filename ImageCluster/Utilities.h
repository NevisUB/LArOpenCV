#ifndef __IMAGECLUSTER_UTILITIES_H__
#define __IMAGECLUSTER_UTILITIES_H__

#include "ImageClusterTypes.h"

namespace larcv {

  ::cv::Rect BoundingBox(const larcv::Contour_t& cluster);
  
  ::cv::Mat CreateSubMatRef(const larcv::Contour_t& cluster, cv::Mat& img);

  ::cv::Mat CreateSubMatCopy(const larcv::Contour_t& cluster, const cv::Mat& img);

}
#endif
