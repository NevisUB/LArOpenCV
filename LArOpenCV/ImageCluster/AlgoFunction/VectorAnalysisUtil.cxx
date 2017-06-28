#ifndef __VECTORANALYSISUTIL_CXX__
#define __VECTORANALYSISUTIL_CXX__

#include "VectorAnalysisUtil.h"
#include <cmath>


namespace larocv {

  float Dot(const std::array<float,3>& p1,
	    const std::array<float,3>& p2) {

    return p1[0]*p2[0] + p1[1]*p2[1] + p1[2]*p2[2];
  }

  std::array<float,3> Sum(const std::array<float,3>& p1,
			  const std::array<float,3>& p2) {

    return {{p1[0]+p2[0], p1[1]+p2[1], p1[2]+p2[2]}};
  }
  
  std::array<float,3> Diff(const std::array<float,3>& p1,
			   const std::array<float,3>& p2) {
    return {{p1[0]-p2[0], p1[1]-p2[1], p1[2]-p2[2]}};
  }

  float QuadSum2(const std::array<float,3>& p1,
		 const std::array<float,3>& p2) {

    auto sum = Sum(p1,p2);
    return sum[0]*sum[0] + sum[1]*sum[1] + sum[2]*sum[2];
  }
  
  float QuadDiff2(const std::array<float,3>& p1,
		  const std::array<float,3>& p2) {
    auto diff = Diff(p1,p2);
    return diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2];
  }

  float Distance(const std::array<float,3>& p1,
		 const std::array<float,3>& p2) {
    return QuadDiff(p1,p2);
  }
  
  float QuadSum(const std::array<float,3>& p1,
		const std::array<float,3>& p2) {
    auto sum2 = QuadSum2(p1,p2);
    return std::sqrt(sum2);
  }

  float QuadDiff(const std::array<float,3>& p1,
		 const std::array<float,3>& p2) {
    auto diff2 = QuadDiff2(p1,p2);
    return std::sqrt(diff2);
  }
 
  float Norm2(const std::array<float,3>& p1) {
    return Dot(p1,p1);
  }
  
  float Norm(const std::array<float,3>& p1) {
    auto norm2 = Norm2(p1);
    return std::sqrt(norm2);
  }

  std::array<float,3> Scale(float a,
			    const std::array<float,3>& p1) {
    return {{a*p1[0],a*p1[1],a*p1[2]}};
  }
  
  std::array<float,3> Scale(const std::array<float,3>& p1,
			    float a) {
    return Scale(a,p1);
  }

  std::array<float,3> AsVector(float theta, float phi) {
    std::array<float,3> res;
    res[0] = std::sin(theta) * std::cos(phi);
    res[1] = std::sin(theta) * std::sin(phi);
    res[2] = std::cos(theta);
    return res;
  }

  std::array<float,3> AsVector(const float x, const float y, const float z) {
    return {{x,y,z}};
  }
  
}  
#endif
