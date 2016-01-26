#ifndef __IMAGECLUSTERTYPES_H__
#define __IMAGECLUSTERTYPES_H__

#include "opencv2/core/mat.hpp"
#include "Core/LArCVTypes.h"

namespace larcv {

  /// larcv::ImageClusterBase inherit algorithm's unique identifier within larcv::ClusterImageManager
  typedef size_t AlgorithmID_t;  
  /// A cluster's unique identifier within larcv::ClusterImageManager
  typedef size_t ClusterID_t;
  /// A 2D contour type
  typedef std::vector<cv::Point2d> Contour_t;
  /// A set of 2D contours
  typedef std::vector<larcv::Contour_t> ContourArray_t;
  /// A constant to signify invalid "id" in general
  static const size_t kINVALID_ID = larcv::kINVALID_SIZE;
  /// A constant to signify, in particular, an invalid larcv::AlgorithmID_t
  static const AlgorithmID_t kINVALID_ALGO_ID = kINVALID_ID;
  /// A constant to signify, in particular, an invalid larcv::ClusterID_t
  static const ClusterID_t kINVALID_CLUSTER_ID = kINVALID_ID;

}
#endif
