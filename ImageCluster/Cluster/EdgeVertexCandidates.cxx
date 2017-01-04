#ifndef __EDGEVERTEXCANDIDATES_CXX__
#define __EDGEVERTEXCANDIDATES_CXX__

#include "EdgeVertexCandidates.h"
#include "Core/Line.h"

namespace larocv {

  /// Global larocv::EdgeVertexCandidatesFactory to register AlgoFactory
  static EdgeVertexCandidatesFactory __global_EdgeVertexCandidatesFactory__;

  void EdgeVertexCandidates::_Configure_(const ::fcllite::PSet &pset)
  {
  }


  void EdgeVertexCandidates::_Process_(const larocv::Cluster2DArray_t& clusters,
				const ::cv::Mat& img,
				larocv::ImageMeta& meta,
				larocv::ROI& roi)
  {
  }

  bool EdgeVertexCandidates::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
  }
  
}
#endif
