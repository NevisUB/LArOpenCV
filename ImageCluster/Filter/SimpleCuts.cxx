#ifndef __SIMPLECUTS_CXX__
#define __SIMPLECUTS_CXX__

#include "SimpleCuts.h"

namespace larocv{

  void SimpleCuts::_Configure_(const ::fcllite::PSet &pset)
  {
    _NHits     = pset.get<int>("MinNHits");
  }

  Cluster2DArray_t SimpleCuts::_Process_(const larocv::Cluster2DArray_t& clusters,
					 const ::cv::Mat& img,
					 larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    Cluster2DArray_t out_clusters = clusters;

    Cluster2DArray_t oclusters; oclusters.reserve(clusters.size());
    for(auto& ocluster : out_clusters) {

      ocluster._numHits = ocluster._insideHits.size();

      if (ocluster._numHits <= _NHits) continue;

      oclusters.emplace_back(ocluster);
      
    }    
    
    return oclusters;
    
  }

}
#endif
