#ifndef __FILLCLUSTERPARAMS_CXX__
#define __FILLCLUSTERPARAMS_CXX__

#include "FillClusterParams.h"

namespace larocv {


  void FillClusterParams::_Configure_(const ::fcllite::PSet &pset)
  {}

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
    }
    
    Contour_t all_locations;
    ::cv::findNonZero(img, all_locations); // get the non zero points
    
     for( const auto& loc: all_locations ) {
      for( size_t i = 0; i < oclusters.size(); i++ ) {
	
	if ( ::cv::pointPolygonTest(oclusters[i]._contour,loc,false) < 0 ) 
	  continue;

	oclusters[i]._insideHits.emplace_back(loc.x, loc.y);
	oclusters[i]._sumCharge += (int) img.at<uchar>(loc.y, loc.x);
      }   
    }
    
    return oclusters;
  }
}

#endif
