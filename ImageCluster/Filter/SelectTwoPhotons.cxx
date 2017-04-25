#ifndef __SELECTTWOPHOTONS_CXX__
#define __SELECTTWOPHOTONS_CXX__

#include "SelectTwoPhotons.h"

namespace larocv{

  void SelectTwoPhotons::_Configure_(const ::fcllite::PSet &pset)
  { }

  Cluster2DArray_t SelectTwoPhotons::_Process_(const larocv::Cluster2DArray_t& clusters,
					 const ::cv::Mat& img,
					 larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    // logic:
    // per plane, only keep the two clusters with the most number of hits


    // if there are 2 or fewer clusters, return them
    if (clusters.size() <= 2) return clusters;

    Cluster2DArray_t oclusters; oclusters.reserve(clusters.size());

    size_t idx_max_00 = 0; // index of largest cluster
    int    hit_max_00 = 0; // cluster with most hits
    size_t idx_max_01 = 0; // index of second largest cluster
    int    hit_max_01 = 0; // cluster with 2nd most hits

    for (size_t i=0; i < clusters.size(); i++) {
      
      auto const& cluster = clusters.at(i);

      if (cluster._numHits() > hit_max_00) {
	hit_max_00 = cluster._numHits();
	idx_max_00 = i;
      }

      else if (cluster._numHits() > hit_max_01) {
	hit_max_01 = cluster._numHits();
	idx_max_01 = i;
      }

    }// for all clusters on this plane

    oclusters.push_back( clusters.at(idx_max_00) );
    oclusters.push_back( clusters.at(idx_max_01) );
    
    return oclusters;
    
  }

}
#endif
