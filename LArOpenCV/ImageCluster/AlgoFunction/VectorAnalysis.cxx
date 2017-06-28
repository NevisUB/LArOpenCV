#ifndef __VECTORANALYSIS_CXX__
#define __VECTORANALYSIS_CXX__

#include "VectorAnalysis.h"
#include <cmath>
#include <array>
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"

namespace larocv {
  
  float CosOpeningAngle(const float theta1, const float phi1,
			const float theta2, const float phi2) {
    

    auto r1 = AsVector(theta1,phi1);
    auto r2 = AsVector(theta2,phi2);
    
    float top = Dot(r1,r2);

    return top;
  }


  float CosOpeningXZAngle(const float theta1, const float phi1,
			  const float theta2, const float phi2) {

    auto r1 = AsVector(theta1,phi1);
    auto r2 = AsVector(theta2,phi2);
    
    float top = r1[0] * r2[0] + r1[2] * r2[2];

    float bot = 1.0;
    bot *= std::sqrt(r1[0]*r1[0] + r1[2]*r1[2]);
    bot *= std::sqrt(r2[0]*r2[0] + r2[2]*r2[2]);

    if (bot == 0.0) return kINVALID_FLOAT;
      
    return top / bot;
    
 
  }

// Holy smokes sorry for stealing this from GeoAlgo, was just too easy...
  std::array<float,3> ClosestPoint(const std::array<float,3>& p1, // point 1 on line
				   const std::array<float,3>& p2, // point 2 on line
				   const std::array<float,3>& p3) // point 3 not on line
  {
    const auto ab = Diff(p2,p1);
    const auto t = Dot( Diff(p3,p1), ab );
    const auto denom = Norm2(ab);
    return Sum( p1, Scale(ab,t/denom) );
  }

  
}

#endif
