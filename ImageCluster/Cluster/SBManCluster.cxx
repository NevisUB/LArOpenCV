#ifndef __SBMANCLUSTER_CXX__
#define __SBMANCLUSTER_CXX__

#include "SBManCluster.h"

// The viewer displays clusters in a single plane a time. For each plane:  
//   There will be some number of contours displayed. Decide how many clusters you want. For each cluster:
//     -Set start and end point by holding mouse down at start of cluster,
//      dragging mouse and releasing on end point.  Only the up, down clicks
//      are saved; your path from start to end does not need to trace the direction. 
//     -Set polygon point by right clicking in the display at each point you
//      want a polygon vertex.  
//     -When you're done with both of the above, type 's' to save the information you've
//      just gathered
//   When you've repeated this process for all the clustering you'd like to do, type 'e' to exit this plane
//   and head to the next.

namespace larocv {

  //Start and end points
  cv::Point start_temp(0,0), end_temp(0,0);
  
  //Polygon
  std::vector<cv::Point> poly_temp ;

  double pix_h(0), pix_w(0), o_x(0), o_y(0) ;

  bool clicked = false;


  void SBManCluster::_Configure_(const ::fcllite::PSet &pset)
  {

    _dilation_size = pset.get<int> ("DilationSize");
    _dilation_iter = pset.get<int> ("DilationIterations");
    
    _blur_size     = pset.get<int> ("BlurSize");
    
    _thresh        = pset.get<float> ("Thresh");
    _thresh_maxval = pset.get<float> ("ThreshMaxVal");
  
  }

  larocv::Cluster2DArray_t SBManCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
					       const ::cv::Mat& img,
					       larocv::ImageMeta& meta)
  {
    pix_h = meta.pixel_height() ;
    pix_w = meta.pixel_width() ;
    o_x = meta.origin().x ;
    o_y = meta.origin().y ;

    if ( clusters.size() )
      throw larbys("This algo can only be executed first in algo chain!");

    ::cv::Mat sb_img; //(s)mooth(b)inary image

    //Dilate
    auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(_dilation_size,_dilation_size));
    ::cv::dilate(img,sb_img,
		 kernel,::cv::Point(-1,-1),_dilation_iter);

    //Blur
    ::cv::blur(sb_img,sb_img,
	       ::cv::Size(_blur_size,_blur_size));

    //std::cout<<"Size of img : "<<sb_img.size() <<std::endl ;
    //::cv::resize(sb_img,resized, cv::Size(sb_img.cols / col_resize, sb_img.rows));

    //Contour finding
    ContourArray_t ctor_v;    
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    ctor_v.clear(); cv_hierarchy_v.clear();
    
    ::cv::findContours(sb_img,ctor_v,cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    


     //Fill some cluster parameters 
     Cluster2DArray_t result_v; result_v.reserve(ctor_v.size());

     // Draw contours, set mouse call function, store start/end points + polygon points
     ////////////////////////////////////////////////////////
     ::cv::RNG rng(12345);
     ::cv::Mat drawing(img) ; //::cv::Mat::zeros( sb_img.size(), CV_8UC3 );
     ::cv::threshold(drawing,img, _thresh,_thresh_maxval,CV_THRESH_BINARY); //return type is "threshold value?" 

     for( int i = 0; i< ctor_v.size(); i++ ){
        ::cv::Scalar color = ::cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        //if( ::cv::contourArea(ctor_v[i]) < 200 ) continue;
        ::cv::drawContours( drawing , ctor_v, i, color, 2, 8, ::cv::noArray(), 0, ::cv::Point() );
      }

     ::cv::imshow( "Contours", drawing);
     ::cv::setMouseCallback("Contours",&onMouse,NULL );
   
     std::vector<cv::Point> start_end_save ;
     std::vector<std::vector<cv::Point>> polygons_save;

     poly_temp.clear() ;
     start_temp.x = 0; start_temp.y = 0; end_temp.x   = 0; end_temp.y   = 0;

     int y_offset(0), x_offset(0) ;
     int y_down(100), x_over(100) ;
     int y_range(900), x_range(900) ;
     ::cv::Mat down_img = drawing ;
      
     while(1){

       // Enter 'e' to exit the image view; enter 's' to save 
       // your start + end points + polygons
       char c = cv::waitKey();

       if(c=='e') break;
       
       if(c == 's'){

           poly_temp.push_back(poly_temp[0]) ;
           start_end_save.push_back(start_temp); start_end_save.push_back(end_temp);
           polygons_save.push_back(poly_temp);

           poly_temp.clear() ;
           start_temp.x = 0; start_temp.y = 0; end_temp.x   = 0; end_temp.y   = 0;

           std::cout<<"Saving polygon and start point! "<<polygons_save.size()<<", "<<start_end_save.size()<<std::endl ;
         }

       // Allows scrolling up and down 
       if( drawing.rows > y_range){
         if(c=='u'){
           y_offset -= y_down ;
           if( y_offset < 0 ){
             down_img = ::cv::Mat(drawing,cv::Range(0,y_range));  
             y_offset = 0;
              }
           else
             down_img = ::cv::Mat(drawing,cv::Range(y_offset, y_offset+ y_range));  
              
             ::cv::imshow( "Contours", down_img);
            } 
         if(c=='d'){
           y_offset+= y_down ;
           if( (y_offset + y_range) > drawing.rows ){
             y_offset -= y_down ;
             down_img = ::cv::Mat(drawing,cv::Range(y_offset, drawing.rows)); //,cv::Range(0,drawing.rows));
             }
           else{
             down_img = ::cv::Mat(drawing,cv::Range(y_offset, y_offset + y_range));  
	     }
              
             ::cv::imshow( "Contours", down_img);

            //std::cout<<"Dealing with range : "<<y_offset<<", "<<y_offset+y_range<<std::endl;
            } 
          }

       // Allows scroll left to right
       if( drawing.cols > x_range){
         if(c == 'l'){
           x_offset -= x_over ;
           if( x_offset < 0 ){
             x_offset = 0;
             down_img = ::cv::Mat(drawing,cv::Range(x_offset,x_range));  
              }
           else
             down_img = ::cv::Mat(drawing,cv::Range(x_offset, x_offset+ x_range));  
              
             ::cv::imshow( "Contours", down_img);
            } 
         if(c == 'r'){
           x_offset += x_over ;
           if( (x_offset + x_range) > drawing.rows ){
             down_img = ::cv::Mat(drawing,cv::Range(x_offset, drawing.rows)); //,cv::Range(0,drawing.rows));
             x_offset -= x_over ;
             }
           else
             down_img = ::cv::Mat(drawing,cv::Range(x_offset, x_offset + x_range));  
              
             ::cv::imshow( "Contours", down_img);

             std::cout<<"Dealing with range : "<<x_offset<<", "<<x_offset+x_range<<std::endl;
            } 
          }


       }

     ::larocv::Cluster2D new_clus ;
     
     for(int j = 0; j < start_end_save.size() / 2; ++j){

       for(int i = 0; i < polygons_save[j].size(); ++i){
         if( y_offset != 0)
           polygons_save[j][i].y += y_offset ;
         if( x_offset != 0)
           polygons_save[j][i].x += x_offset ;
          }

       cv::RotatedRect rect0 = ::cv::minAreaRect(cv::Mat(polygons_save[j]));
       cv::Point2f vertices[4];
       rect0.points(vertices);
       auto rect = rect0.size;
       std::vector<cv::Point2f> rectangle = { vertices[0], vertices[1],vertices[2],vertices[3] };
       std::swap(new_clus._minAreaRect,rectangle);

       new_clus._area = ::cv::contourArea(polygons_save[j]) ;
       new_clus._perimeter = ::cv::arcLength(polygons_save[j],1);
       new_clus._length    = rect.height > rect.width ? rect.height : rect.width;
       new_clus._width     = rect.height > rect.width ? rect.width  : rect.height;
       new_clus._numHits   = 0 ;
       new_clus._sumCharge = 0 ;
       std::swap(new_clus._contour,polygons_save[j]);

      if( y_offset != 0){
        new_clus._startPt.y = start_end_save[2 * j].y + y_offset ;
        new_clus._endPt.y   = start_end_save[2 * j + 1].y + y_offset ; 
         }
       else{ 
         new_clus._startPt.y = start_end_save[2 * j].y ; 
         new_clus._endPt.y   = start_end_save[2 * j + 1].y ;
         }
       
       if( x_offset != 0){
         new_clus._startPt.x = start_end_save[2 * j].x + x_offset ;
         new_clus._endPt.x   = start_end_save[2 * j + 1].x + x_offset ;
         }
       else{
         new_clus._startPt.x = start_end_save[2 * j].x;
         new_clus._endPt.x   = start_end_save[2 * j + 1].x ;
         }

       //std::cout<<"0) Start point is: "<<new_clus._startPt.x<<", "<<new_clus._startPt.y <<std::endl ;
       result_v.push_back(new_clus);
       }

     Contour_t all_locations;
     ::cv::findNonZero(drawing, all_locations); // get the non zero points

     for( const auto& loc: all_locations ) {
       for( size_t i = 0; i < result_v.size(); i++ ) {

         if ( ::cv::pointPolygonTest(result_v[i]._contour,loc,false) < 0 ) 
           continue;

         result_v[i]._insideHits.emplace_back(loc.x, loc.y);
         result_v[i]._numHits ++ ;
         result_v[i]._sumCharge += (int) img.at<uchar>(loc.y, loc.x);  
          }   
        }   

       for( size_t i = 0; i < result_v.size(); i++){

         float min_st_dist = 1000000. ;
         int min_st_i = -1;
         float min_end_dist = 1000000. ;
         int min_end_i = -1;

         for( size_t j = 0; j < result_v[i]._insideHits.size(); j++){ 

          auto & loc = result_v[i]._insideHits[j] ;

          //auto st = ( 0.5 + loc.x ) * meta.pixel_height() + meta.origin().y;
          //auto sw = ( 0.5 + loc.y ) * meta.pixel_width() + meta.origin().x;
          //std::cout<<"Hits at : "<<st<<", "<<sw<<std::endl;

          float dist_st  = sqrt(pow(loc.x - result_v[i]._startPt.x,2) + pow(loc.y - result_v[i]._startPt.y,2)); 
          float dist_end = sqrt(pow(loc.x - result_v[i]._endPt.x,2) + pow(loc.y - result_v[i]._endPt.y,2)); 

          if( dist_st < min_st_dist){
            min_st_dist = dist_st ;
            min_st_i = j; 
           }
          if( dist_end < min_end_dist){
            min_end_dist = dist_end ;
            min_end_i = j; 
           }
         }
         if( min_st_i != -1 && min_end_i != -1){
         result_v[i]._startPt.x = result_v[i]._insideHits[min_st_i].x ;
         result_v[i]._startPt.y = result_v[i]._insideHits[min_st_i].y ;
         result_v[i]._endPt.x = result_v[i]._insideHits[min_end_i].x ;
         result_v[i]._endPt.y = result_v[i]._insideHits[min_end_i].y ;

         }
         auto st = ( 0.5 + result_v[i]._startPt.x ) * meta.pixel_height() + meta.origin().y;
         auto sw = ( 0.5 + result_v[i]._startPt.y ) * meta.pixel_width() + meta.origin().x;

         //std::cout<<"1) Hit adjusted start is: "<<result_v[i]._startPt.x<<", "<<result_v[i]._startPt.y <<std::endl ;
         //std::cout<<"start time and wire  "<<st<<", "<<sw <<std::endl;
       }

     std::cout<<"Result : "<<result_v.size()<<std::endl ;

    return result_v;
  }


  void onMouse( int event, int x, int y, int f,void * ptr ){

    //Here we can drag and draw start_temp is stored on click, move mouse, release stores end_temp.

    switch(event){

        case  CV_EVENT_LBUTTONDOWN  :
          //std::cout<<"Setting start point!: "<<x<<", "<<y<<std::endl;
          start_temp.x=x;
          start_temp.y=y;
	  std::cout<<"Setting time/wire!: "<< ( 0.5 + start_temp.x ) * pix_h + o_y<<", "<<( 0.5 + start_temp.y ) * pix_w + o_x<<std::endl;
          break;

        case  CV_EVENT_LBUTTONUP    :
          std::cout<<"Setting end point!: "<<x<<", "<<y<<std::endl ;
          end_temp.x=x;
          end_temp.y=y;
          break;
         
        case  CV_EVENT_RBUTTONDOWN  :
          std::cout<<"Adding point to polygon contour!"<<x<<", "<<y<<std::endl;
          poly_temp.push_back(cv::Point(x,y));
          break;

        default :   
          break;

    }

  }

}

#endif
