#ifndef __RECOPARAMETERS_CXX__
#define __RECOPARAMETERS_CXX__

#include "RecoParameters.h"

namespace larocv {

  void RecoParameters::_Configure_(const ::fcllite::PSet &pset)
  {}

  Cluster2DArray_t RecoParameters::_Process_(const larocv::Cluster2DArray_t& clusters,
					     const ::cv::Mat& img,
					     larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    Cluster2DArray_t oclusters; oclusters.reserve( clusters.size() );

    for (const auto& cluster : clusters) {

      auto ocluster = cluster;

      ocluster._startPt       = ocluster.reco.startpt;
      ocluster._endPt         = ocluster.reco.endpt;
      ocluster._eigenVecFirst = ocluster.reco.dir;

      oclusters.emplace_back(ocluster);
    }

    return oclusters;

  }

}
#endif
