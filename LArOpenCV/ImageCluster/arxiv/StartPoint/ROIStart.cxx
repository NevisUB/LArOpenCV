#ifndef __ROISTART_CXX__
#define __ROISTART_CXX__

#include "ROIStart.h"

namespace larocv {

  /// Global larocv::ROIStartFactory to register AlgoFactory
  static ROIStartFactory __global_ROIStartFactory__;

  void ROIStart::_Configure_(const Config_t &pset)
  {
     _min_vertex_dist = pset.get<float>("MinVtxDist") ;
  }

  Cluster2DArray_t ROIStart::_Process_(const larocv::Cluster2DArray_t& clusters,
                                       const ::cv::Mat& img,
                                       larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    Cluster2DArray_t roi_clusters; roi_clusters.reserve(clusters.size());

    auto pi0st = roi.roivtx_in_image(meta);
    
    LAROCV_DEBUG() << "In image coords: (" << pi0st.x << "," << pi0st.y << ")\n";

    for (size_t k = 0; k < clusters.size(); k++) {

      Cluster2D cluster = clusters[k]; // make copy

      if ( !cluster._insideHits.size() ) continue; //throw larbys();

      float min_dist = std::pow(10,9);
      float max_dist = 0;
      int  min_hit_index = -1 ;
      int  max_hit_index = -1 ;

      auto const & hits = cluster._insideHits ;

      for (int i = 0; i < hits.size(); i++) {

        auto & hit = hits[i] ;

        auto dist = sqrt( pow(pi0st.x - hit.x, 2) + pow(pi0st.y - hit.y, 2) );

        /// Find smallest distance between all hits and ROI vertex point
        /// Make closest hit the start point
        if ( dist < min_dist ) {
          min_dist = dist ;
          min_hit_index = i ;
        }

        if ( dist > max_dist ) {
          max_dist = dist ;
          max_hit_index = i ;
        }

      }

      auto& roi = cluster.roi;

      roi.startpt.x = hits[min_hit_index].x;
      roi.startpt.y = hits[min_hit_index].y;

      // std::cout << " ROI start :("<<roi.startpt.x<<","<<roi.startpt.y<<")\n";

      roi.endpt.x   = hits[max_hit_index].x;
      roi.endpt.y   = hits[max_hit_index].y;

      // std::cout << " ROI end :("<<roi.endpt.x<<","<<roi.endpt.y<<")\n";

      roi.dist = std::sqrt( std::pow(roi.startpt.x - pi0st.x, 2) + std::pow(roi.startpt.y - pi0st.y, 2) );

      // std::cout<<" roi distance : " <<roi.dist<<std::endl;
      // std::cout<<" and the far distance is " << std::sqrt( std::pow(roi.endpt.x - pi0st.x, 2) + std::pow(roi.endpt.y - pi0st.y, 2) ) << "\n";

      if ( roi.dist < _min_vertex_dist ){
        roi.dir.x = ( roi.endpt.x - roi.startpt.x ) / roi.dist;
        roi.dir.y = ( roi.endpt.y - roi.startpt.y ) / roi.dist;
      }
      else {
        roi.dir.x = ( roi.startpt.x - pi0st.x ) / roi.dist;
        roi.dir.y = ( roi.startpt.y - pi0st.y ) / roi.dist;
      }

      roi.vtx = Point2D(pi0st.x, pi0st.y);

      roi_clusters.emplace_back( cluster );

    }

    return roi_clusters;
  }


}
#endif
