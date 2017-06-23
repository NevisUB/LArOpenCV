#ifndef __VECTORANALYSIS_H__
#define __VECTORANALYSIS_H__

#include "VectorAnalysisUtil.h"

namespace larocv {

  float CosOpeningAngle(const float theta1, const float phi1,
			const float theta2, const float phi2);

  float CosOpeningXZAngle(const float theta1, const float phi1,
			  const float theta2, const float phi2);
  
  std::array<float,3> ClosestPoint(const std::array<float,3>& p1,  // point 1 on line
				   const std::array<float,3>& p2,  // point 2 on line
				   const std::array<float,3>& p3); // point 3 not on line
}

#endif
