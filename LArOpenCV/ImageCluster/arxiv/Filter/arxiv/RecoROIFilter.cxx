#ifndef __RECOROIFILTER_CXX__
#define __RECOROIFILTER_CXX__

#include "RecoROIFilter.h"

namespace larocv {

  /// Global larocv::RecoROIFilterFactory to register AlgoFactory
  static RecoROIFilterFactory __global_RecoROIFilterFactory__;

  void RecoROIFilter::_Configure_(const ::fcllite::PSet& pset) {
    _max_rad_length = pset.get<double>("MaxRadLength");  // minimum distance for ROI vertex to start point

    _maxclusters = pset.get<int>("MaxClusters");  // how many is too many
    _minclusters = pset.get<int>("MinClusters");  // how many is too many

    _strict_hit_cut = pset.get<int>("StrictHitCut");  // how many is too many
  }

  Cluster2DArray_t RecoROIFilter::_Process_(const larocv::Cluster2DArray_t& clusters,
					    const ::cv::Mat& img,
					    larocv::ImageMeta& meta, larocv::ROI& roi) {
    Cluster2DArray_t oclusters;
    oclusters.reserve(clusters.size());

    for (const auto& cluster : clusters) {

      if (cluster.roi.dist > _max_rad_length || cluster._numHits() < _strict_hit_cut)  // start point distance to roi vertex
	  continue;

      auto ocluster = cluster;  // make a copy

      ocluster._startPt = ocluster.roi.startpt;
      ocluster._endPt   = ocluster.roi.endpt;

      ocluster._eigenVecFirst = ocluster.reco.dir;

      oclusters.emplace_back(ocluster);
    }
    
    if (oclusters.size() > _maxclusters) { // to many clusters moving on to matching
      //std::cout << "too many clusters @ " << oclusters.size() << "\n";
	return Cluster2DArray_t();
      }

    if (oclusters.size() < _minclusters) {  // to little clusters moving on to matching
      //std::cout << "to little clusters @ " << oclusters.size() << "\n";
      return Cluster2DArray_t();
    }

    return oclusters;
  }
}
#endif
