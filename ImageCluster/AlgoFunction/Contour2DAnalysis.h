#ifndef __CONTOUR2DANALYSIS_H_
#define __CONTOUR2DANALYSIS_H_

#include "Base/ImageClusterTypes.h"

namespace larocv {

  cv::Mat CleanImage(const cv::Mat& img,
		     const GEO2D_ContourArray_t& veto_ctor_v,
		     float pi_threshold=0);
  
}
#endif
