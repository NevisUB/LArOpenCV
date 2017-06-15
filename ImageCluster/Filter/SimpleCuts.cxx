#ifndef __SIMPLECUTS_CXX__
#define __SIMPLECUTS_CXX__

#include "SimpleCuts.h"

namespace larocv{

  void SimpleCuts::_Configure_(const ::fcllite::PSet &pset)
  {
    _min_hits      = pset.get<int>("MinHits");
    _min_area      = pset.get<float>("MinArea");
    _min_perimeter = pset.get<float>("MinPerimieter");
    _min_charge    = pset.get<int>("MinCharge");
    _min_length    = pset.get<float>("MinLength");
    _min_width     = pset.get<float>("MinWidth");
    _pool          = pset.get<bool>("UsePool");
    _min_pool_hits = pset.get<int>("MinPoolHits");
				     
  }

  Cluster2DArray_t SimpleCuts::_Process_(const larocv::Cluster2DArray_t& clusters,
					 const ::cv::Mat& img,
					 larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    Cluster2DArray_t oclusters; oclusters.reserve(clusters.size());

    Cluster2DArray_t remove_clus; remove_clus.reserve(clusters.size());

    for(auto& cluster : clusters) {

      if ( _pool && cluster._num_hits_pool < _min_pool_hits ){
        remove_clus.emplace_back(cluster);
        continue; 
      }

      if (cluster._numHits() < _min_hits)      continue;
      if (cluster._perimeter < _min_area)      continue;
      if (cluster._area      < _min_perimeter) continue;
      if (cluster._sumCharge < _min_charge)    continue;
      if (cluster._length    < _min_length)    continue;
      if (cluster._width     < _min_width)     continue;

      oclusters.emplace_back(cluster);
      
    }    

    // Great! We've filtered. But we don't want to lose the hits associated to small clusters
    // that overlap with the removed clusters. To solve, we loop through all the hits of removed
    // clusters, and check whether they fall inside any other contour
    std::vector<::cv::Point> all_locations;

    for(auto& cluster : remove_clus) {
      for(auto& loc : cluster._insideHits) 
         all_locations.emplace_back(::cv::Point(loc.x,loc.y));
    }
   
    ::cv::Mat pool_meta = img;

    if ( _pool )
      pool_meta = meta.get_pool_meta() ;

    for( const auto& loc: all_locations ) {

      for(auto& cluster : oclusters) {

        if ( ::cv::pointPolygonTest(cluster._contour,loc,false) < 0 ) continue;

        if( _pool )
          cluster._num_hits_pool += pool_meta.at<uchar>(loc.y, loc.x);

        cluster._insideHits.emplace_back(loc.x, loc.y);
        cluster._sumCharge += (int) img.at<uchar>(loc.y, loc.x);

        // When point is found in contour, no others are checked; avoids double counting
        // Requires SimpleCuts to the alg chain after this; may have clusters with 0 hits
        break;
      }
    }

    // Finally, reset the image grid to contain info about only the hits 
    // left in current clusters (eg, clusters with only 2/3 hits are removed above; 
    // their hits will no longer make it into the merging pool)
    // Get remaining locations with meta.get_locations() 
    meta.clear_locations();

    for(auto& cluster : oclusters) {
      for(auto const & i : cluster._insideHits )
        meta.add_location(i);
    }    

    //for ( auto const & c : oclusters) {
    //  if ( c._num_hits_pool > 15 && c._num_hits_pool < 70 )
    //    std::cout<<"SimpleCuts, cluster size: "<<c._num_hits_pool <<", "<<meta.plane()<<std::endl ;
    //}

    return oclusters;
    
  }

}
#endif
