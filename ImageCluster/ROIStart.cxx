#ifndef __ROISTART_CXX__
#define __ROISTART_CXX__

#include "ROIStart.h"

namespace larcv{

  void ROIStart::_Configure_(const ::fcllite::PSet &pset)
  {}

  Cluster2DArray_t ROIStart::_Process_(const larcv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larcv::ImageMeta& meta)
  { 

    Cluster2DArray_t roi_clusters; roi_clusters.reserve(clusters.size());


    auto pi0_st = meta.roivtx();
    if ( pi0_st.x == ::larcv::kINVALID_DOUBLE ) { std::cout << "BAD VTX\n"; throw std::exception(); }
    // std::cout << "Got VTX: (" << pi0_st.x << "," << pi0_st.y << ")\n";

    auto pi0st = Point2D( (pi0_st.y - meta.origin().y)/meta.pixel_height(), (pi0_st.x - meta.origin().x)/meta.pixel_width() );
    // std::cout << "In image coords: (" << pi0st.x << "," << pi0st.y << ")\n";
    
    for(size_t k = 0; k < clusters.size(); k++){
    
      Cluster2D cluster = clusters[k]; // make copy

      if( !cluster._insideHits.size() ) continue; //throw larbys();

      float min_dist = 1e9;
      float max_dist = 0;
      int  min_hit_index = -1 ;
      int  max_hit_index = -1 ;

      auto const & hits = cluster._insideHits ;

      for(int i = 0; i < hits.size(); i++){

	auto & hit = hits[i] ;
	
	auto dist = sqrt( pow(pi0st.x - hit.x,2) + pow(pi0st.y - hit.y,2) );
      
	/// Find smallest distance between all hits and ROI vertex point
	/// Make closest hit the start point
	if( dist < min_dist ){
	  min_dist = dist ; 
	  min_hit_index = i ;
	}

	if( dist > max_dist ){
	  max_dist = dist ; 
	  max_hit_index = i ;
	}

      }

      auto& roi = cluster.roi;
      
      roi.startpt.x = hits[min_hit_index].x;
      roi.startpt.y = hits[min_hit_index].y; 

      roi.endpt.x   = hits[max_hit_index].x;
      roi.endpt.y   = hits[max_hit_index].y;

      roi.dist = std::sqrt( std::pow(roi.startpt.x - pi0st.x,2) + std::pow(roi.startpt.y - pi0st.y,2) );

      if ( roi.dist < 1. ){
        roi.dir.x = ( roi.endpt.x - roi.startpt.x ) / roi.dist;
        roi.dir.y = ( roi.endpt.y - roi.startpt.y ) / roi.dist;
        }
      else{
        roi.dir.x = ( roi.startpt.x - pi0st.x ) / roi.dist;
        roi.dir.y = ( roi.startpt.y - pi0st.y ) / roi.dist;
      }

      roi.vtx = Point2D(pi0st.x,pi0st.y);
      
      roi_clusters.emplace_back( cluster );
      
    }
  
    return roi_clusters; 
  }


}
#endif
