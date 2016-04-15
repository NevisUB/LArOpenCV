#ifndef __ROIPARAMETERS_CXX__
#define __ROIPARAMETERS_CXX__

#include "ROIParameters.h"

namespace larcv{

  void ROIParameters::_Configure_(const ::fcllite::PSet &pset)
  {}

  Cluster2DArray_t ROIParameters::_Process_(const larcv::Cluster2DArray_t& clusters,
					   const ::cv::Mat& img,
					   larcv::ImageMeta& meta)
  {
    Cluster2DArray_t oclusters; oclusters.reserve( clusters.size() );

    for(const auto& cluster : clusters) {

      auto ocluster = cluster;


      ocluster._startPt      = ocluster.roi.startpt;
      ocluster._endPt        = ocluster.roi.endpt;
      ocluster._eigenVecFirst = ocluster.roi.dir;

      oclusters.emplace_back(ocluster);
    }

    return oclusters;
  }

}
#endif
