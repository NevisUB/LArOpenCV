#ifndef __TCLUSTER_CXX__
#define __TCLUSTER_CXX__

#include "TCluster.h"

namespace larcv{

  void TCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _blur_x          = pset.get<int> ("BlurX");
    _blur_y          = pset.get<int> ("BlurY");
    _thresh        = pset.get<float> ("Thresh");
    _threshMaxVal  = pset.get<float> ("ThreshMaxVal");
    _size_x        = pset.get<float> ("ResizeX");
    _size_y        = pset.get<float> ("ResizeY");
  }

  ContourArray_t TCluster::_Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  { 

    //std::cout<<"Mat width height : "<<img.size()<<std::endl ;
    //std::cout<<"Meta width and height: "<<meta.width()<<", "<<meta.height()<<std::endl ;
  
    ::cv::Mat resized, blurred_img;
    //::cv::resize(img,resized,cv::Size(_size_x,_size_y));
    //::cv::blur(resized,blurred_img,::cv::Size(_blur_x,_blur_y) );
    ::cv::blur(img,blurred_img,::cv::Size(_blur_x,_blur_y) );
    ::cv::threshold(blurred_img,blurred_img,_thresh,_threshMaxVal,CV_THRESH_BINARY);
    ContourArray_t result;
    std::vector<cv::Vec4i> cv_hierarchy_v;
    ::cv::findContours(blurred_img,result,cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); 
    //::cv::resize(result,result,img.Size()); //cv::Size(meta.height(),meta.width()));

    //::cv::imshow("Original",img) ;
    //::cv::imshow("Blurred",blurred_img) ;

    ::cv::RNG rng; //(12345);
    ::cv::Mat drawing = ::cv::Mat::zeros( blurred_img.size(), CV_8UC3 );

    for( int i = 0; i< result.size(); i++ ){
    ::cv::Scalar color = ::cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    if( ::cv::contourArea(result[i]) < 75 ) continue; 
    ::cv::drawContours( drawing , result, i, color, 2, 8, ::cv::noArray(), 0, ::cv::Point() );
   // ::cv::drawContours( drawing, result, i, color) ;
    std::cout<<"Area is : "<<::cv::contourArea(result[i])<<std::endl;
//    _filler.Fill(result[i]);

    }
    ::cv::imshow( "Contours", drawing);
    ::cv::waitKey(0);


    return result;
  
   }  
  
  

}
#endif
