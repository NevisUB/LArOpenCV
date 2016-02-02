#ifndef __TRACKSHOWER_CXX__
#define __TRACKSHOWER_CXX__

#include "TrackShower.h"

namespace larcv{

  void TrackShower::_Configure_(const ::fcllite::PSet &pset)
  {

    _area_separation  = pset.get<int> ("AreaCut");
    _ratio_separation = pset.get<int> ("RatioCut");
    _shower_track_sat = pset.get<int>("ShowerTrackSat");

  }


  ContourArray_t TrackShower::_Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  { 

   ContourArray_t satellite_v ;
   ContourArray_t shower_v ;
   ContourArray_t track_v ;

   for(auto const & cv_contour : clusters){

        cv::RotatedRect rect0 = cv::minAreaRect(cv::Mat(cv_contour));
        cv::Point2f vertices[4];
        rect0.points(vertices);
        auto rect = rect0.size; 
        auto area   = ::cv::contourArea(cv_contour);

        //  
        // Between points 0,1 and 1,2 , find max distance; this will be outer
        // loop. At each walk along length (outer loop), we'll walk along the 
        // width and eventually store max and min widths.
        //  
        int step1 = 80; 
        int step2 = 80; 
        float maxDist = 0;  
        float minDist = 0;  
        std::pair<float,float> dir1;
        std::pair<float,float> dir2;

        auto dist1  = pow(pow(vertices[0].x-vertices[1].x,2) + pow(vertices[0].y - vertices[1].y,2),0.5);
        auto dist2  = pow(pow(vertices[2].x-vertices[1].x,2) + pow(vertices[2].y - vertices[1].y,2),0.5);

        if( dist1 > dist2 ){
          maxDist = dist1;
          minDist = dist2;
          dir1 = std::make_pair((vertices[0].x - vertices[1].x)/step1, (vertices[0].y - vertices[1].y)/step1);
          dir2 = std::make_pair((vertices[2].x - vertices[1].x)/step2, (vertices[2].y - vertices[1].y)/step2);
  
           }
         else{
          maxDist = dist2;
          minDist = dist1;
          dir1 = std::make_pair((vertices[2].x - vertices[1].x)/step1, (vertices[2].y - vertices[1].y)/step1);
          dir2 = std::make_pair((vertices[0].x - vertices[1].x)/step2, (vertices[0].y - vertices[1].y)/step2);
           }

        float dist_travelled = 0 ;
        float max_width = 0;
        float min_width = 10000;

        ::cv::Point2d new_point1 ;
        ::cv::Point2d new_point2 ;

        for(int i=1; i<step1-1; i+=2){
            new_point1.x = vertices[1].x + i*dir1.first ;
            new_point1.y = vertices[1].y + i*dir1.second ;
            dist_travelled = 0 ;

            for(int j=0; j<step2; j+=1){
              new_point2.x = new_point1.x+ j*dir2.first ;
              new_point2.y = new_point1.y+ j*dir2.second ;

              if ( ::cv::pointPolygonTest(cv_contour,new_point2,false) >= 0 )
                dist_travelled += minDist/step2 ;
              }
            if( dist_travelled > max_width )
              max_width = dist_travelled ;

            if( dist_travelled < min_width && dist_travelled != 0)
              min_width = dist_travelled ;
          }

        if( area > _area_separation && (max_width/min_width) > _ratio_separation)
         shower_v.push_back(cv_contour);
        else if( area > _area_separation && (max_width/min_width) < _ratio_separation)
         track_v.push_back(cv_contour);
        else
         satellite_v.push_back(cv_contour);
      }

   std::cout<<"Shower, track, satellite size: "<<shower_v.size()<<", "<<track_v.size()<<", "<<satellite_v.size()<<std::endl ;
 
  if( _shower_track_sat == 1)
    return shower_v ; 
  else if( !_shower_track_sat )
    return track_v ;
  else 
    return satellite_v; 
  }

}
#endif
