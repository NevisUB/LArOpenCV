#ifndef __VECTORANALYSISUTIL_H__
#define __VECTORANALYSISUTIL_H__

#include <array>
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"

namespace larocv {

  float Dot(const std::array<float,3>& p1,const std::array<float,3>& p2);

  std::array<float,3> Sum(const std::array<float,3>& p1,const std::array<float,3>& p2);
  std::array<float,3> Diff(const std::array<float,3>& p1,const std::array<float,3>& p2);
  
  float QuadSum(const std::array<float,3>& p1,const std::array<float,3>& p2);
  float QuadDiff(const std::array<float,3>& p1,const std::array<float,3>& p2);

  float QuadSum2(const std::array<float,3>& p1,const std::array<float,3>& p2);
  float QuadDiff2(const std::array<float,3>& p1,const std::array<float,3>& p2);

  float Distance(const std::array<float,3>& p1, const std::array<float,3>& p2);
		 
  float Norm2(const std::array<float,3>& p1);
  float Norm(const std::array<float,3>& p1);

  std::array<float,3> Scale(float a, const std::array<float,3>& p1);
  std::array<float,3> Scale(const std::array<float,3>& p1,float a);

  std::array<float,3> AsVector(const float theta, const float phi);
  std::array<float,3> AsVector(const float x, const float y, const float z);
}

#endif
