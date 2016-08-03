#ifndef __TRACKSHOWER_CXX__
#define __TRACKSHOWER_CXX__

#include "TrackShower.h"

namespace larocv{

  void TrackShower::_Configure_(const ::fcllite::PSet &pset)
  {

    _area_cut         = pset.get<int> ("AreaCut");
    _ratio_separation = pset.get<int> ("RatioCut");
    _track_shower_sat = pset.get<int> ("TrackShowerSat");

    _step1 = pset.get<int>("Step1");
    _step2 = pset.get<int>("Step2");
    // _switch_ratio = pset.get<int>("SwitchRatio");

  }

  Cluster2DArray_t TrackShower::_Process_(const larocv::Cluster2DArray_t& clusters,
					  const ::cv::Mat& img,
					  larocv::ImageMeta& meta)
  { 

    Cluster2DArray_t ts_clusters; ts_clusters.reserve(clusters.size());

    Cluster2DArray_t shower_v, track_v, satellite_v, shower_and_track_v;
      
    for(size_t k = 0; k < clusters.size(); k++){
       
      auto& cv_contour = clusters[k]._contour;

      Cluster2D ts_cluster = clusters[k];

      cv::RotatedRect rect0 = ::cv::minAreaRect(cv::Mat(cv_contour));
      cv::Point2f vertices[4];
      rect0.points(vertices);
      
      ::cv::Point2d new_point1, new_point2;

      float max_width = 0;
      float min_width = 10000;

      float max_long_dist = 0;
      float min_long_dist = 0;

      float dist_travelled = 0 ;
      float long_dist_travelled = 0;

      int step1 = _step1;
      int step2 = _step2; 

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
      
      auto dir1 = std::make_pair((vertices[i0].x - vertices[i1].x)/step1, (vertices[i0].y - vertices[i1].y)/step1);
      auto dir2 = std::make_pair((vertices[i2].x - vertices[i1].x)/step2, (vertices[i2].y - vertices[i1].y)/step2);
      
      
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
	  max_width     = dist_travelled ;
	  max_long_dist = long_dist_travelled;
	}

	if( dist_travelled < min_width && dist_travelled != 0){
	  min_long_dist = long_dist_travelled;
	  min_width     = dist_travelled ;
	}
      }
      
      ts_cluster._max_width = max_width ;
      ts_cluster._min_width = min_width ;

      ts_clusters.push_back(ts_cluster);

      //std::cout << "\t>> area: " << ts_cluster._area << " _area_cut: " << _area_cut << "\n";
      if( ts_cluster._area > _area_cut ){

	shower_and_track_v.push_back(ts_cluster);

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

    switch ( _track_shower_sat ) {

    case 0: return shower_v;
    case 1: return track_v;
    case 2: return satellite_v;
    case 3: return ts_clusters;

    default: return shower_and_track_v;
      
    }
    
    // if ( _track_shower_sat == 0 )
    //   return shower_v;
    // else if( _track_shower_sat == 1) 
    //   return track_v;
    // else if( _track_shower_sat == 2) 
    //   return satellite_v;
    // else if( _track_shower_sat == 3)
    //   return ts_clusters;
    // else
    //   return shower_and_track_v;
   
  }

}
#endif
