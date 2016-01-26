#ifndef __IMAGECLUSTER_UTILITIES_CXX__
#define __IMAGECLUSTER_UTILITIES_CXX__

#include <limits.h>
#include <opencv2/imgproc/imgproc.hpp>
#include "Core/larbys.h"
#include "Utilities.h"
namespace larcv {

  ::cv::Rect BoundingBox(const larcv::Contour_t& cluster)
  {
    if(cluster.size()<2)
      throw larbys("Cannot create SubMatrix for a contour of size < 2!");
    
    int x_min = INT_MAX;
    int y_min = INT_MAX;
    int x_max = 0;
    int y_max = 0;

    for(auto const& pt : cluster) {

      if(pt.x < x_min) x_min = pt.x;
      if(pt.x > x_max) x_max = pt.x;
      if(pt.y < y_min) y_min = pt.y;
      if(pt.y > y_max) y_max = pt.y;

    }

    return ::cv::Rect( x_min, y_min,
		       x_max - x_min + 1,
		       y_max - y_min + 1);
  }

  ::cv::Mat CreateSubMatRef(const larcv::Contour_t& cluster, cv::Mat& img)
  {
    if(cluster.size()<2)
      throw larbys("Cannot create SubMatrix for a contour of size < 2!");

    auto bbox = BoundingBox(cluster);

    ::cv::Mat result;
    img(bbox).copyTo(result);

    return result;
  }

  ::cv::Mat CreateSubMatCopy(const larcv::Contour_t& cluster, const cv::Mat& img)
  {
    if(cluster.size()<2)
      throw larbys("Cannot create SubMatrix for a contour of size < 2!");

    auto bbox = BoundingBox(cluster);

    ::cv::Mat result = ::cv::Mat::zeros(img.size(),img.type());

    for(int x=0; x<img.rows; ++x) {

      for(int y=0; y<img.cols; ++y) {

	int submat_x = x - bbox.x;
	int submat_y = y - bbox.y;

	double inside = ::cv::pointPolygonTest(cluster,::cv::Point2f(x,y),false);

	if(inside<0) continue;

	result.at<float>(submat_x,submat_y,0) = img.at<float>(x,y,0);
      }

    }

    return result;
  }

}

#endif
