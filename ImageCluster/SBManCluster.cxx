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

namespace larcv {

  //Start and end points
  cv::Point start_temp(0,0), end_temp(0,0);
  
  //Polygon
  std::vector<cv::Point> poly_temp ;

  bool clicked = false;


  void SBManCluster::_Configure_(const ::fcllite::PSet &pset)
  {

    _dilation_size = pset.get<int> ("DilationSize");
    _dilation_iter = pset.get<int> ("DilationIterations");
    
    _blur_size     = pset.get<int> ("BlurSize");
    
    _thresh        = pset.get<float> ("Thresh");
    _thresh_maxval = pset.get<float> ("ThreshMaxVal");
  
  }

  larcv::Cluster2DArray_t SBManCluster::_Process_(const larcv::Cluster2DArray_t& clusters,
					       const ::cv::Mat& img,
					       larcv::ImageMeta& meta)
  {

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

    // Images that go off screen are not handled properly yet. Need to think some more
    // This means the resize block is not working right now, but should only affeect resized blocks
    ////////////////////////////////////////////////////////////////////////////////////////
    ::cv::Mat resized(sb_img);
    int row_resize = 4; 
    int col_resize = 10; 
    bool row_set = false; 
    bool col_set = false; 
     
    std::cout<<"Size of img : "<<sb_img.size() <<std::endl ;
    if ( sb_img.rows > 1000){
      ::cv::resize(sb_img,resized, cv::Size(sb_img.cols, sb_img.rows / row_resize));
      row_set = true;
      row_resize = float(sb_img.rows) / row_resize ; 
     }
    if ( sb_img.cols > 1000){
      ::cv::resize(sb_img,resized, cv::Size(sb_img.cols / col_resize, sb_img.rows));
      col_set = true;
      }
    std::cout<<"Size of img : "<<resized.size() <<std::endl ;
    ////////////////////////////////////////////////////////////////////////////////////////

    //Contour finding
    ContourArray_t ctor_v;    
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    ctor_v.clear(); cv_hierarchy_v.clear();
    
    ::cv::findContours(resized,ctor_v,cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);


     //Fill some cluster parameters 
     Cluster2DArray_t result_v; result_v.reserve(ctor_v.size());

     // Draw contours, set mouse call function, store start/end points + polygon points
     ////////////////////////////////////////////////////////
     ::cv::RNG rng(12345);
     ::cv::Mat drawing = ::cv::Mat::zeros( sb_img.size(), CV_8UC3 );

     for( int i = 0; i< ctor_v.size(); i++ ){
        ::cv::Scalar color = ::cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        if( ::cv::contourArea(ctor_v[i]) < 200 ) continue;
        ::cv::drawContours( drawing , ctor_v, i, color, 2, 8, ::cv::noArray(), 0, ::cv::Point() );
      }

     ::cv::imshow( "Contours", drawing);
     ::cv::setMouseCallback("Contours",&onMouse,NULL );
   
     std::vector<cv::Point> start_end_save ;
     std::vector<std::vector<cv::Point>> polygons_save;

     poly_temp.clear() ;
     start_temp.x = 0; start_temp.y = 0; end_temp.x   = 0; end_temp.y   = 0;

     std::cout<<"Entering loop "<<std::endl ;

     while(1){

       // Enter 'e' to exit the image view; enter 's' to save 
       // your start + end points + polygons
       char c = cv::waitKey();
       if(c=='e') break;
       
       if(c == 's'){
         for(int i = 0; i < poly_temp.size(); i++) {
           if(row_set){ poly_temp[i].y *= row_resize ; }
           if(col_set){ poly_temp[i].x *= col_resize ; }
             }
           if(row_set){ start_temp.y *= row_resize ; end_temp.y *= row_resize ; }
           if(col_set){ start_temp.x *= col_resize ; end_temp.x *= col_resize ; }

           start_end_save.push_back(start_temp); start_end_save.push_back(end_temp);
           polygons_save.push_back(poly_temp);

           std::cout<<"Saving polygon and start point! "<<std::endl ;
         }
       }
    ////////////////////////////////////////////////////////

     ::larcv::Cluster2D new_clus ;
     
     for(size_t j = 0; j < start_end_save.size() / 2; ++j){

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

       new_clus._startPt.x = start_end_save[2 * j].x ;
       new_clus._startPt.y = start_end_save[2 * j].y;

       new_clus._endPt.x = start_end_save[2 * j + 1].x ;
       new_clus._endPt.y = start_end_save[2 * j + 1].y;
       
       result_v.push_back(new_clus);
       }
    
     Contour_t all_locations;
     ::cv::findNonZero(img, all_locations); // get the non zero points

     for( const auto& loc: all_locations ) {
       for( size_t i = 0; i < result_v.size(); i++ ) {

         if ( ::cv::pointPolygonTest(result_v[i]._contour,loc,false) < 0 ) 
           continue;

         result_v[i]._insideHits.emplace_back(loc.x, loc.y);
         result_v[i]._numHits ++ ;
         result_v[i]._sumCharge += (int) img.at<uchar>(loc.y, loc.x);  
          }   
        }   

     std::cout<<"Result : "<<result_v.size()<<std::endl ;

    return result_v;
  }


  void onMouse( int event, int x, int y, int f,void * ptr ){

    //Here we can drag and draw start_temp is stored on click, move mouse, release stores end_temp.
    //std::cout<<"In onMouse ! "<<std::endl ;
    

    switch(event){

        case  CV_EVENT_LBUTTONDOWN  :
          std::cout<<"Setting start point!";
          start_temp.x=x;
          start_temp.y=y;
          break;

        case  CV_EVENT_LBUTTONUP    :
          std::cout<<"Setting end point!";
          end_temp.x=x;
          end_temp.y=y;
          break;
         
        case  CV_EVENT_RBUTTONDOWN  :
          std::cout<<"Adding point to polygon contour!";
          poly_temp.push_back(cv::Point(x,y));
          break;

        default :   
          break;

    }

  }

}

#endif
