#ifndef __SIMPLECUTS_CXX__
#define __SIMPLECUTS_CXX__

#include "SimpleCuts.h"

namespace larocv{

  /// Global larocv::SimpleCutsFactory to register AlgoFactory
  static SimpleCutsFactory __global_SimpleCutsFactory__;

  void SimpleCuts::_Configure_(const ::fcllite::PSet &pset)
  {
    _min_hits      = pset.get<int>("MinHits");
    _min_area      = pset.get<float>("MinArea");
    _min_perimeter = pset.get<float>("MinPerimieter");
    _min_charge    = pset.get<int>("MinCharge");
    _min_length    = pset.get<float>("MinLength");
    _min_width     = pset.get<float>("MinWidth");
				     
  }

  Cluster2DArray_t SimpleCuts::_Process_(const larocv::Cluster2DArray_t& clusters,
					 const ::cv::Mat& img,
					 larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    Cluster2DArray_t oclusters; oclusters.reserve(clusters.size());

    for(auto& cluster : clusters) {

      if (cluster._numHits() < _min_hits)      continue;
      if (cluster._perimeter < _min_area)      continue;
      if (cluster._area      < _min_perimeter) continue;
      if (cluster._sumCharge < _min_charge)    continue;
      if (cluster._length    < _min_length)    continue;
      if (cluster._width     < _min_width)     continue;
      
      oclusters.emplace_back(cluster);
      
    }    
    
    return oclusters;
    
  }

}
#endif
