#ifndef __TRACKSHOWER_CXX__
#define __TRACKSHOWER_CXX__

#include "TrackShower.h"

namespace larcv{

  void TrackShower::_Configure_(const ::fcllite::PSet &pset)
  {

    _area_separation  = pset.get<int> ("AreaCut");
    _ratio_separation = pset.get<int> ("RatioCut");
    _track_shower_sat = pset.get<int>("TrackShowerSat");

    if(!_contour_tree){
      _contour_tree = new TTree("contour_tree","Contour Tree" );
      _contour_tree->Branch("area",&_area,"area/F");
      _contour_tree->Branch("perimeter",&_perimeter,"perimeter/F");
      _contour_tree->Branch("bb_height",&_bb_height,"bb_height/F");
      _contour_tree->Branch("bb_width",&_bb_width,"bb_width/F");
      _contour_tree->Branch("max_con_width",&_max_con_width,"max_con_width/F");
      _contour_tree->Branch("min_con_width",&_min_con_width,"min_con_width/F");
      _contour_tree->Branch("angle",&_angle,"angle/F");
    }
    
  }

  Cluster2DArray_t TrackShower::_Process_(const larcv::Cluster2DArray_t& clusters,
					  const ::cv::Mat& img,
					  larcv::ImageMeta& meta)
  { 

   _cparms_v.clear();
   _cparms_v.reserve(clusters.size());

   ContourArray_t satellite_v ;
   ContourArray_t shower_v ;
   ContourArray_t track_v ;

   for(size_t k = 0; k < clusters.size(); k++){
       
      //auto cv_contour = clusters[k];
      auto& cv_contour = clusters[k]._contour;

      auto area = ::cv::contourArea(cv_contour);

      //Get hits 
      //::cv::Rect rect00 = ::cv::boundingRect(cv_contour);
      //::cv::Mat subMat(img, rect00);

      std::vector<::cv::Point> locations;
      ::cv::findNonZero(img, locations);  
      //::cv::findNonZero(subMat, locations);  

      auto contour_temp = cv_contour;
      //for(auto &pt : contour_temp) { pt.x -= rect00.x; pt.y -= rect00.y; }
      
      std::vector<std::pair<int,int> > hits;
      int nhits = 0;
    
      for(const auto& loc : locations) {
        if ( ::cv::pointPolygonTest(contour_temp, loc,false) < 0 ) 
          continue;
   
        //hits.emplace_back(loc.x + rect00.x, loc.y + rect00.y);
        hits.emplace_back(loc.x, loc.y);
        ++nhits;
      }   

        cv::RotatedRect rect0 = cv::minAreaRect(cv::Mat(cv_contour));
        cv::Point2f vertices[4];
        rect0.points(vertices);
        auto rect = rect0.size; 

        auto perimeter = ::cv::arcLength(cv_contour,1);
        auto bb_height = ( rect.height > rect.width ? rect.height : rect.width );
        auto bb_width  = ( rect.height > rect.width ? rect.width : rect.height );

        std::vector<cv::Point2f> rectangle = { vertices[0], vertices[1],vertices[2],vertices[3] };

        _area      = area;  
        _perimeter = perimeter;
        _bb_height = bb_height ;
        _bb_width  = bb_width;

        if(area < 900) continue;

        //  
        // Between points 0,1 and 1,2 , find max distance; this will be outer
        // loop. At each walk along length (outer loop), we'll walk along the 
        // width and eventually store max and min widths.
        //  
        int step1 = 80; 
        int step2 = 80; 
        std::pair<float,float> dir1;
        std::pair<float,float> dir2;

        auto dist1  = pow(pow(vertices[0].x-vertices[1].x,2) + pow(vertices[0].y - vertices[1].y,2),0.5);
        auto dist2  = pow(pow(vertices[2].x-vertices[1].x,2) + pow(vertices[2].y - vertices[1].y,2),0.5);

        float maxDist = 0;  
        float minDist = 0;  

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

        ::cv::Point2d new_point1 ;
        ::cv::Point2d new_point2 ;

        float max_width = 0;
        float min_width = 10000;

        float dist_travelled = 0 ;
	float max_long_dist = 0;
	float min_long_dist = 0;
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
        if( max_long_dist /(maxDist / 2) < 0.349 ) {
          auto temp = start_point ;
	  start_point = end_point ;
	  end_point = temp;
          switched = 1 ;
	  //if(area > 900)
	  //std::cout<<"Swtiching!"<<std::endl;
	  }

        std::vector<cv::Point2d> find_end;
        std::vector<cv::Point2d> find_start ;

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

        cv::Point2d new_start; 
        cv::Point2d new_end ;

	//findNewPoint(new_start,find_start);
          
         for(int k = 0; k < find_start.size(); k++){
           new_start.x += find_start[k].x / find_start.size();
           new_start.y += find_start[k].y / find_start.size() ;
            }

         for(int k = 0; k < find_end.size(); k++){
           new_end.x += find_end[k].x / find_end.size();
           new_end.y += find_end[k].y / find_end.size() ;
            }


        std::vector< std::pair<double,double> > start_end ;     
        start_end.push_back(std::make_pair(new_start.x,new_start.y));
        start_end.push_back(std::make_pair(new_end.x,new_end.y));
	
         
	 if(area > 900){
	   _angle = 180 / 3.14 * atan2(new_end.y - new_start.y, new_end.x - new_start.x ) ;
	   if( _angle < 0 )
	     _angle = (360 + _angle)  ;
           //std::cout<<"Percent : "<<(max_long_dist)/(maxDist/2)*100<<"\%, "<<area<<std::endl ;
	   }

        _max_con_width = max_width;
        _min_con_width = min_width;

        if( area > _area_separation && (max_width/min_width) >= _ratio_separation)
         shower_v.push_back(cv_contour);
        else if( area > _area_separation && (max_width/min_width) < _ratio_separation)
         track_v.push_back(cv_contour);
        else
         satellite_v.push_back(cv_contour);
      
        _contour_tree->Fill();

     // this is the only way you can get shit into python at this point
      _cparms_v.emplace_back(k,
                             _area,
                             _perimeter,
                             dir1.first/maxDist,
                             dir1.second/maxDist,
                             nhits,
                             hits,
                             start_end,
                             rectangle,
			     _angle
                             );  
 
      }

//   std::cout<<"Shower, track, satellite size: "<<shower_v.size()<<", "<<track_v.size()<<", "<<satellite_v.size()<<std::endl ;
   
   ContourArray_t shower_sats_v; shower_sats_v.reserve(shower_v.size() + satellite_v.size());
   for(auto& shower : shower_v)    shower_sats_v.push_back( shower );
   for(auto& sats   : satellite_v) shower_sats_v.push_back( sats  );
	 

   Cluster2DArray_t result;

   if( _track_shower_sat == 2) {
     result.resize(shower_sats_v.size());
     for(size_t i=0; i<shower_sats_v.size(); ++i) std::swap(result[i]._contour, shower_sats_v[i]);
     return result;
   }
   else if( _track_shower_sat == 1) {
     result.resize(shower_v.size());
     for(size_t i=0; i<shower_v.size(); ++i) std::swap(result[i]._contour, shower_v[i]);
     return result;
   }
   else if( _track_shower_sat == 0 ) {
     result.resize(track_v.size());
     for(size_t i=0; i<track_v.size(); ++i) std::swap(result[i]._contour, track_v[i]);
     return result;
   }
   else if( _track_shower_sat == 3) return clusters;
   else {
     result.resize(satellite_v.size());
     for(size_t i=0; i<satellite_v.size(); ++i) std::swap(result[i]._contour, satellite_v[i]);
     return result;
   }
   
  }

 void TrackShower::Finalize(TFile* fout){
   
   if(fout){
     fout->cd();
     _contour_tree->Write();
      }
  }
  
}
#endif
