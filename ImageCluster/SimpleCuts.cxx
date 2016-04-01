#ifndef __SIMPLECUTS_CXX__
#define __SIMPLECUTS_CXX__

#include "SimpleCuts.h"

namespace larcv{

  void SimpleCuts::_Configure_(const ::fcllite::PSet &pset)
  {
    _NHits     = pset.get<int>("MinNHits");
    _NClusters = pset.get<int>("MinNClusters");
  }

  Cluster2DArray_t SimpleCuts::_Process_(const larcv::Cluster2DArray_t& clusters,
					 const ::cv::Mat& img,
					 larcv::ImageMeta& meta)
  {

    Cluster2DArray_t out_clusters = clusters;

    Contour_t all_locations;
    ::cv::findNonZero(img, all_locations); // get the non zero points

    for(const auto& loc: all_locations) {
      for(auto& ocluster : out_clusters) {
	if ( ::cv::pointPolygonTest(ocluster._contour,loc,false) < 0 ) continue;
	
	ocluster._insideHits.emplace_back(loc.x, loc.y);
      }
    }

    Cluster2DArray_t oclusters; oclusters.reserve(clusters.size());
    for(auto& ocluster : out_clusters) {

      ocluster._numHits = ocluster._insideHits.size();

      if (ocluster._numHits <= _NHits) continue;

      oclusters.emplace_back(ocluster);
      
    }    
    
    // Are there enough NCUT clusters? No return
    if ( oclusters.size() < _NClusters )
      return Cluster2DArray_t ();
    
    return oclusters;
    
  }

}
#endif
