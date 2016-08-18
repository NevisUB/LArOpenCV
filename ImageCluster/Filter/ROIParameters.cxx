#ifndef __ROIPARAMETERS_CXX__
#define __ROIPARAMETERS_CXX__

#include "ROIParameters.h"

namespace larocv {

  void ROIParameters::_Configure_(const ::fcllite::PSet &pset)
  {}

  Cluster2DArray_t ROIParameters::_Process_(const larocv::Cluster2DArray_t& clusters,
					    const ::cv::Mat& img,
					    larocv::ImageMeta& meta, larocv::ROI& roi)
  {
    //Assigned ROI parameters to start point, end point, and direction

    Cluster2DArray_t oclusters;
    oclusters.reserve( clusters.size() );

    for (const auto& cluster : clusters) {

      auto ocluster = cluster;
      ocluster._startPt       = ocluster.roi.startpt;
      ocluster._endPt         = ocluster.roi.endpt;
      ocluster._eigenVecFirst = ocluster.roi.dir;

      oclusters.emplace_back(ocluster);
    }

    return oclusters;
  }

}
#endif
