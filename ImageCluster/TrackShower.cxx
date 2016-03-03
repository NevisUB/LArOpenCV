#ifndef __TRACKSHOWER_CXX__
#define __TRACKSHOWER_CXX__

#include "TrackShower.h"

namespace larcv{

  void TrackShower::_Configure_(const ::fcllite::PSet &pset)
  {

    _area_cut  = pset.get<int> ("AreaCut");
    _ratio_separation = pset.get<int> ("RatioCut");
    _track_shower_sat = pset.get<int>("TrackShowerSat");

    _step1 = pset.get<int>("Step1");
    _step2 = pset.get<int>("Step2");

    _switch_ratio = pset.get<int>("SwitchRatio");

  }

  Cluster2DArray_t TrackShower::_Process_(const larcv::Cluster2DArray_t& clusters,
					  const ::cv::Mat& img,
					  larcv::ImageMeta& meta)
  { 

    Cluster2DArray_t ts_clusters; ts_clusters.reserve(clusters.size());

    Cluster2DArray_t shower_v, track_v, satellite_v;

    std::vector<::cv::Point> locations;
    ::cv::findNonZero(img, locations);  
      
    for(size_t k = 0; k < clusters.size(); k++){
       
      auto& cv_contour = clusters[k]._contour;

      Cluster2D ts_cluster = clusters[k];

      auto area = ::cv::contourArea(cv_contour);

      int sum_charge = 0;
	
      for(const auto& loc : locations) {
        if ( ::cv::pointPolygonTest(cv_contour, loc,false) < 0 ) 
          continue;
   
        ts_cluster._insideHits.emplace_back(loc.x, loc.y);	

	int charge  = (int) img.at<uchar>(loc.y,loc.x);
	sum_charge += charge;					 
      }   

      ts_cluster._numHits = ts_cluster._insideHits.size();
	
      cv::RotatedRect rect0 = ::cv::minAreaRect(cv::Mat(cv_contour));
      cv::Point2f vertices[4];
      rect0.points(vertices);
      auto rect = rect0.size;

      std::vector<cv::Point2f> rectangle = { vertices[0], vertices[1],vertices[2],vertices[3] };
      std::swap(ts_cluster._minAreaRect,rectangle);
      
      //  
      // Between points 0,1 and 1,2 , find max distance; this will be outer
      // loop. At each walk along length (outer loop), we'll walk along the 
      // width and eventually store max and min widths.
      //  
      int step1 = _step1; 
      int step2 = _step2; 
      std::pair<float,float> dir1;
      std::pair<float,float> dir2;

      auto dist1  = std::sqrt(pow(vertices[0].x-vertices[1].x,2) + pow(vertices[0].y - vertices[1].y,2));
      auto dist2  = std::sqrt(pow(vertices[2].x-vertices[1].x,2) + pow(vertices[2].y - vertices[1].y,2));

      float maxDist(0), minDist(0);

      int i0(0), i1(1), i2(2);

      if( dist1 > dist2 ){
	maxDist = dist1;
	minDist = dist2;
      }
      else{
	maxDist = dist2;
	minDist = dist1;
	i0 = 2; i2 = 0;
      }

      dir1 = std::make_pair((vertices[i0].x - vertices[i1].x)/step1, (vertices[i0].y - vertices[i1].y)/step1);
      dir2 = std::make_pair((vertices[i2].x - vertices[i1].x)/step2, (vertices[i2].y - vertices[i1].y)/step2);
      ::cv::Point2d start_point( vertices[i1].x , vertices[i1].y  );
      ::cv::Point2d end_point  ( vertices[i0].x , vertices[i0].y );

      ::cv::Point2d new_point1, new_point2;

      float max_width = 0;
      float min_width = 10000;

      float max_long_dist = 0;
      float min_long_dist = 0;

      float dist_travelled = 0 ;
      float long_dist_travelled = 0;

      for(int i=1; i<step1-1; i+=2){
	long_dist_travelled += maxDist/step1 ;
	new_point1.x = vertices[1].x + i*dir1.first ;
	new_point1.y = vertices[1].y + i*dir1.second ;
	dist_travelled = 0 ;

	for(int j=0; j<step2; j+=1){
	  new_point2.x = new_point1.x+ j*dir2.first ;
	  new_point2.y = new_point1.y+ j*dir2.second ;

	  if ( ::cv::pointPolygonTest(cv_contour,new_point2,false) >= 0 )
	    dist_travelled += minDist/step2 ;
	}
	if( dist_travelled > max_width ){
	  max_width = dist_travelled ;
	  max_long_dist = long_dist_travelled;
	}

	if( dist_travelled < min_width && dist_travelled != 0){
	  min_long_dist = long_dist_travelled;
	  min_width = dist_travelled ;
	}
      }

      bool switched = 0;
      if( max_long_dist / (maxDist / 2) < _switch_ratio ) {
	auto temp   = start_point ;
	start_point = end_point ;
	end_point   = temp;
	switched    = 1 ;
      }

      std::vector<cv::Point2d> find_end, find_start;

      int it = 0; 
      while((!find_end.size() || !find_start.size()) && it < 10){

	start_point.x += pow(-1,switched)*dir1.first ;
	start_point.y += pow(-1,switched)*dir1.second ;

	end_point.x -= pow(-1,switched)*dir1.first ;
	end_point.y -= pow(-1,switched)*dir1.second ;

	for(int j=0; j<step2; j+=1){
            
	  start_point.x += dir2.first ;
	  start_point.y += dir2.second;

	  end_point.x += dir2.first ;
	  end_point.y += dir2.second;

	  if ( ::cv::pointPolygonTest(cv_contour,start_point,false) >= 0 )
	    find_start.push_back(start_point);

	  if ( ::cv::pointPolygonTest(cv_contour,end_point,false) >= 0 )
	    find_end.push_back(end_point);
	}
	it++;
      }

      Point2D new_start, new_end; 

      double angle; 

      for(int k = 0; k < find_start.size(); k++){
	new_start.x += find_start[k].x / find_start.size();
	new_start.y += find_start[k].y / find_start.size() ;
      }

      for(int k = 0; k < find_end.size(); k++){
	new_end.x += find_end[k].x / find_end.size();
	new_end.y += find_end[k].y / find_end.size() ;
      }

      angle = 180 / 3.14 * atan2(new_end.y - new_start.y, new_end.x - new_start.x ) ;
      if( angle < 0 )
	angle += 360 ;
      //std::cout<<"Percent : "<<(max_long_dist)/(maxDist/2)*100<<"\%, "<<area<<std::endl ;
      
      ts_cluster._startPt   = new_start; 
      ts_cluster._endPt     = new_end;  
      ts_cluster._angle2D   = angle;
      ts_cluster._sumCharge = sum_charge;
      ts_cluster._length    = rect.height > rect.width ? rect.height : rect.width;
      ts_cluster._width     = rect.height > rect.width ? rect.width : rect.height;
      ts_cluster._area      = area;
      ts_cluster._perimeter = ::cv::arcLength(cv_contour,1);

      ts_clusters.push_back(ts_cluster);

      if( area > _area_cut){
	if ( max_width/min_width >= _ratio_separation)
	  shower_v.push_back(ts_cluster);
	else 
	  track_v.push_back(ts_cluster);
      }
      else
	satellite_v.push_back(ts_cluster);
    }

    //   std::cout<<"Shower, track, satellite size: "<<shower_v.size()<<", "<<track_v.size()<<", "<<satellite_v.size()<<std::endl ;
   
    Cluster2DArray_t shower_sats_v; shower_sats_v.reserve(shower_v.size() + satellite_v.size());

    for(auto& shower : shower_v)    shower_sats_v.push_back( shower );
    for(auto& sats   : satellite_v) shower_sats_v.push_back( sats  );

    if ( _track_shower_sat == 0 )
      return shower_v;
    else if( _track_shower_sat == 1) 
      return track_v;
    else if( _track_shower_sat == 2) 
      return satellite_v;
    else 
      return ts_clusters;
   
  }

}
#endif
