#ifndef __SMOOTHBINARYCLUSTER_CXX__
#define __SMOOTHBINARYCLUSTER_CXX__

#include "SmoothBinaryCluster.h"
#include "Core/larbys.h"
#include <opencv2/imgproc/imgproc.hpp>
namespace larcv{

  void SmoothBinaryCluster::Configure(const ::fcllite::PSet &pset)
  {

    _dilation_size = pset.get<int> ("DilationSize");
    _dilation_iter = pset.get<int> ("DilationIterations");
    
    _blur_size     = pset.get<int> ("BlurSize");
    
    _thresh        = pset.get<float> ("Thresh");
    _thresh_maxval = pset.get<float> ("ThreshMaxVal");

    // _polygon_e     = pset.get<double>("PolygonEpsilon");
  }

  ContourArray_t SmoothBinaryCluster::_Process_(const larcv::ContourArray_t& clusters,
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

    //Threshold
    auto t_value = ::cv::threshold(sb_img,sb_img,
				   _thresh,_thresh_maxval,CV_THRESH_BINARY); //return type is "threshold value?"


    //Contour finding
    ContourArray_t ctor_v;    
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    ctor_v.clear(); cv_hierarchy_v.clear();
    
    ::cv::findContours(sb_img,ctor_v,cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    
    //Approximate the polygon curve using Douglas-Peucker algorithm
    
    // ContourArray_t ctor_result_v;
    // ctor_result_v.resize(ctor_v.size());

    // for( size_t k = 0; k < ctor_v.size(); k++ )
    //   ::cv::approxPolyDP(ctor_v[k], ctor_result_v[k], _polygon_e, true); //true to close the contours


    // return ctor_result_v;
    return ctor_v;
    
  }

}
#endif
