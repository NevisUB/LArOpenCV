#ifndef __TCLUSTER_CXX__
#define __TCLUSTER_CXX__

#include "TCluster.h"

namespace larocv{

  void TCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _blur_x          = pset.get<int> ("BlurX");
    _blur_y          = pset.get<int> ("BlurY");
    _thresh        = pset.get<float> ("Thresh");
    _threshMaxVal  = pset.get<float> ("ThreshMaxVal");
    _size_x        = pset.get<float> ("ResizeX");
    _size_y        = pset.get<float> ("ResizeY");
  }

  Cluster2DArray_t TCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta,
				       larocv::ROI& roi)
  { 

    //std::cout<<"Mat width height : "<<img.size()<<std::endl ;
    //std::cout<<"Meta width and height: "<<meta.width()<<", "<<meta.height()<<std::endl ;
  
    ::cv::Mat resized, blurred_img;
    //::cv::resize(img,resized,cv::Size(_size_x,_size_y));
    //::cv::blur(resized,blurred_img,::cv::Size(_blur_x,_blur_y) );
    ::cv::blur(img,blurred_img,::cv::Size(_blur_x,_blur_y) );
    ::cv::threshold(blurred_img,blurred_img,_thresh,_threshMaxVal,CV_THRESH_BINARY);
    ContourArray_t ctor_v;
    std::vector<cv::Vec4i> cv_hierarchy_v;
    ::cv::findContours(blurred_img,ctor_v,cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); 
    //::cv::resize(ctor_v,ctor_v,img.Size()); //cv::Size(meta.height(),meta.width()));

    //::cv::imshow("Original",img) ;
    //::cv::imshow("Blurred",blurred_img) ;

    ::cv::RNG rng; //(12345);
    ::cv::Mat drawing = ::cv::Mat::zeros( blurred_img.size(), CV_8UC3 );

    for( int i = 0; i< ctor_v.size(); i++ ){
    ::cv::Scalar color = ::cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    if( ::cv::contourArea(ctor_v[i]) < 75 ) continue; 
    ::cv::drawContours( drawing , ctor_v, i, color, 2, 8, ::cv::noArray(), 0, ::cv::Point() );
   // ::cv::drawContours( drawing, ctor_v, i, color) ;
    std::cout<<"Area is : "<<::cv::contourArea(ctor_v[i])<<std::endl;

    }
    ::cv::imshow( "Contours", drawing);
    ::cv::waitKey(0);

    Cluster2DArray_t result;
    result.resize(ctor_v.size());

    for(size_t i=0; i<ctor_v.size(); ++i) std::swap(result[i]._contour,ctor_v[i]);

    return result;
  
   }  
  
  

}
#endif
