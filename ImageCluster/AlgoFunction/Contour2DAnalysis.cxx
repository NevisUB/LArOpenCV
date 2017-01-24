#ifndef __CONTOUR2DANALYSIS_CXX_
#define __CONTOUR2DANALYSIS_CXX_

#include "Contour2DAnalysis.h"
#include "Core/VectorArray.h"
#include "opencv2/imgproc.hpp"

namespace larocv {
  cv::Mat CleanImage(const cv::Mat& img,
		     const GEO2D_ContourArray_t& veto_ctor_v,
		     float pi_threshold)
  {
    cv::Mat thresh_img;
    ::cv::threshold(img,thresh_img,pi_threshold,1000,3);
    geo2d::VectorArray<int> points;
    findNonZero(thresh_img, points);
    for(auto const& pt : points) {
      for(auto const& veto_ctor : veto_ctor_v) {
	auto dist = ::cv::pointPolygonTest(veto_ctor,pt,false);
	if(dist<0) continue;
	thresh_img.at<unsigned char>(pt.y,pt.x) = 0;
	break;
      }
    }
    return thresh_img;
  }

}
#endif
