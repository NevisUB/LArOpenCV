#ifndef __IMAGECLUSTER_UTILITIES_H__
#define __IMAGECLUSTER_UTILITIES_H__

#include "ImageClusterTypes.h"

namespace larcv {

  ::cv::Rect BoundingBox(const larcv::ContourArray_t& clusters, const double min_area=-1);

  ::cv::Rect BoundingBox(const larcv::Contour_t& cluster);
  
  ::cv::Mat CreateSubMatRef(const larcv::Contour_t& cluster, cv::Mat& img);

  ::cv::Mat CreateSubMatCopy(const larcv::Contour_t& cluster, const cv::Mat& img);

  unsigned int Factorial(unsigned int n);
  
  std::vector<std::vector<size_t> > ClusterCombinations(const std::vector<size_t>& seed);

  std::vector<std::vector<std::pair<size_t,size_t> > > PlaneClusterCombinations(const std::vector<size_t>& seed);


}
#endif
