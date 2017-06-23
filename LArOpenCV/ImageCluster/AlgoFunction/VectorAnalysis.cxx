#ifndef __VECTORANALYSIS_CXX__
#define __VECTORANALYSIS_CXX__

#include "VectorAnalysis.h"
#include <cmath>
#include <array>

namespace larocv {
  
  float CosOpeningAngle(const float theta1, const float phi1,
			const float theta2, const float phi2) {
    
    
    std::array<float,3> r1,r2;

    r1[0] = std::sin(theta1) * std::cos(phi1);
    r1[1] = std::sin(theta1) * std::sin(phi1);
    r1[2] = std::cos(theta1);
    
    r2[0] = std::sin(theta2) * std::cos(phi2);
    r2[1] = std::sin(theta2) * std::sin(phi2);
    r2[2] = std::cos(theta2);
    
    
    float top = 0.0;
    for(short i=0;i<3;++i) top += r1[i]*r2[i];

    return top;
  }
  
}

#endif
