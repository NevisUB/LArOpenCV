#ifndef __FILLCLUSTERPARAMS_CXX__
#define __FILLCLUSTERPARAMS_CXX__

#include "FillClusterParams.h"

namespace larocv {


  void FillClusterParams::_Configure_(const ::fcllite::PSet &pset)
  {
   _pool = pset.get<bool>("UsePool");

  }

  larocv::Cluster2DArray_t FillClusterParams::_Process_(const larocv::Cluster2DArray_t& clusters,
							const ::cv::Mat& img,
							larocv::ImageMeta& meta,
							larocv::ROI& roi)
  {
    auto oclusters = clusters;

    for (auto& ocluster : oclusters) {

      auto& contour = ocluster._contour;
      
      ocluster._minAreaBox  = ::cv::minAreaRect(contour);
      ocluster._boundingBox = ::cv::boundingRect(contour);
      
      auto& min_rect      = ocluster._minAreaBox;
      auto& bounding_rect = ocluster._boundingBox;
      
      ::cv::Point2f vertices[4];

      //rotated rect coordinates
      min_rect.points(vertices);
      ocluster._minAreaRect     = {vertices[0],vertices[1],vertices[2],vertices[3]};

      //axis aligned rect coordinates
      ocluster._minBoundingRect = {bounding_rect.br(),bounding_rect.tl()};

      auto rect = min_rect.size;
      ocluster._area      = ::cv::contourArea(contour) ;
      ocluster._perimeter = ::cv::arcLength(contour,1);
      ocluster._length    = rect.height > rect.width ? rect.height : rect.width;
      ocluster._width     = rect.height > rect.width ? rect.width  : rect.height;
      ocluster._sumCharge = 0 ;
      ocluster._angle2D   = min_rect.angle;
      ocluster._centerPt  = Point2D(min_rect.center.x,min_rect.center.y);
      ocluster._num_hits_pool= 0 ;
    }
    
    std::vector<::cv::Point> all_locations;
    bool first_fill = false ;

    // We store the state of the image (ie, pixels associated with contours
    // that we want to keep) in the image meta info. If a cluster is removed in a filter at some
    // point, and FillClusterParams is called again to update, we don't want removed contours' hits
    // to show up when we're associated pixels to remaining contours.
    // Without this check, we sometimes see decent clusters combined with a distant small clump of
    // 2/3 hits whose original contour was removed by SimpleCuts . This in turn screws up matching.
    if ( !meta.get_locations().size() ){
      ::cv::findNonZero(img, all_locations); 
      first_fill = true; 
    }
    else{ 
      all_locations = meta.get_locations(); 
      for( size_t i = 0; i < oclusters.size(); i++ ){
        oclusters[i]._insideHits.clear() ;
        oclusters[i]._sumCharge = 0;
      }
    }

    // pool_meta is only important if you have pooling enabled.
    // pool_meta has the same dimensions as the argument "img"; the only difference 
    // is that pool_meta's grid contains info on how many pixels were combined at the 
    // pooling stage for each pixel; this prevents number-of-hit info from being lost
    // when we pool
    ::cv::Mat pool_meta = img;

    if ( _pool )
      pool_meta = meta.get_pool_meta() ;

    for( const auto& loc: all_locations ) {
      
      for( size_t i = 0; i < oclusters.size(); i++ ) {

          if ( ::cv::pointPolygonTest(oclusters[i]._contour,loc,false) < 0 ) continue;

	  if( first_fill ) meta.add_location(loc) ;

          if ( _pool )
	    oclusters[i]._num_hits_pool += pool_meta.at<uchar>(loc.y, loc.x);

          oclusters[i]._insideHits.emplace_back(loc.x, loc.y);
          oclusters[i]._sumCharge += (int) img.at<uchar>(loc.y, loc.x);

	  //std::cout<<loc.x<<", "<<loc.y<<", ";

          // When point is found in contour, no others are checked; avoids double counting
          // Requires SimpleCuts to the alg chain after this; may have clusters with 0 hits
          break;
        }   
      }

      //for( size_t i = 0; i < oclusters.size(); i++ ){ 
      //  if( oclusters[i]._num_hits_pool >= 10 ) 
      //    std::cout<<"FillClusterParams Number of hits "<<oclusters[i]._num_hits_pool<<", "<<meta.plane()<<std::endl ;
      //}

    return oclusters;
  }
}

#endif
