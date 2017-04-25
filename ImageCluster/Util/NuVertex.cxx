#ifndef __NUVERTEX_CXX__
#define __NUVERTEX_CXX__

#include "NuVertex.h"
#include <stdio.h>

// check this:
// http://docs.opencv.org/2.4/doc/tutorials/features2d/trackingmotion/harris_detector/harris_detector.html

namespace larocv {


  void NuVertex::_Configure_(const ::fcllite::PSet &pset)
  {

    _ctr = 0;

    _block_size    = pset.get<int>("BlockSize");
    _aperture_size = pset.get<int>("ApertureSize");
    _harris_k      = pset.get<double>("HarrisK");
    _threshold      = pset.get<double>("Threshold");

    _dilation_size = pset.get<int>("DilationSize");
    _dilation_iter = pset.get<int>("DilationIterations");
    
    _blur_size_r = pset.get<int>("BlurSizeR");
    _blur_size_t = pset.get<int>("BlurSizeT");
    
    _thresh        = pset.get<float>("Thresh");
    _thresh_maxval = pset.get<float>("ThreshMaxVal");

  }

  larocv::Cluster2DArray_t NuVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
					       const ::cv::Mat& img,
					       larocv::ImageMeta& meta,
					       larocv::ROI& roi)
  {

    if ( clusters.size() )
      throw larbys("This algo can only be executed first in algo chain!");
    
    ::cv::Mat dst, dst_norm, dst_norm_scaled;

    //get the vertex in the image
    auto pi0st = roi.roivtx_in_image(meta);

    std::vector<::cv::Point> all_locations;
    ::cv::findNonZero(img, all_locations);

    //Blur
    ::cv::blur(img,img,::cv::Size(_blur_size_r,_blur_size_t));

    //Dilate
    auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE, ::cv::Size(_dilation_size,_dilation_size));
    ::cv::dilate(img,img, kernel,::cv::Point(-1,-1),_dilation_iter);
    
    //Threshold
    auto t_value = ::cv::threshold(img, img, _thresh, _thresh_maxval, CV_THRESH_BINARY); //return type is "threshold value?"

    

    std::cout << "start cornerHarris" << std::endl;
    
    /// Detecting corners
    ::cv::cornerHarris( img, dst, _block_size, _aperture_size, _harris_k, ::cv::BORDER_DEFAULT );

    std::vector<int> imgparams;
    std::ostringstream imgname;
    imgname << "img_event_" << _ctr << "_pl_" << meta.plane() << ".jpg";
    imgparams.push_back( CV_IMWRITE_JPEG_QUALITY );
    imgparams.push_back( 100 );
    if (meta.plane() == 2)
      ::cv::imwrite(imgname.str(),img,imgparams);
    
    std::cout << "end cornerHarris" << std::endl;

    std::cout << "start normalize" << std::endl;

    /// Normalizing
    ::cv::normalize( dst, dst_norm, 0, 255, ::cv::NORM_MINMAX, CV_32FC1, ::cv::Mat() );
    ::cv::convertScaleAbs( dst_norm, dst_norm_scaled );

    std::cout << "end normalize" << std::endl;

    ::larlite::user_info uinfo{};

    std::stringstream ss, ss1, ss2;
    ss << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_clusters";
    uinfo.store("ID",ss.str());

    std::cout << "there are " << dst_norm.rows << " points" << std::endl;

    ss1 << "kink_x_";
    ss2 << "kink_y_";
    
    /// Drawing a circle around corners
    for( int j = 0; j < dst_norm.rows ; j++ )
      { for( int i = 0; i < dst_norm.cols; i++ )
	  {
	    if( (int) dst_norm.at<float>(j,i) > _threshold )
	      {
		circle( dst_norm_scaled, ::cv::Point( i, j ), 5,  ::cv::Scalar(0), 2, 8, 0 );


		double x = meta.XtoTimeTick(i);
		double y = meta.YtoWire(j);

		std::cout << "\t above threshold @ " << x << ", " << y << std::endl;
		
		uinfo.append(ss1.str(),x);
		uinfo.append(ss2.str(),y);
		
	      }
	  }
      }

    std::ostringstream cornername;
    cornername << "corner_event_" << _ctr << "_pl_" << meta.plane() << ".jpg";
    if (meta.plane() == 2)
      ::cv::imwrite(cornername.str(),dst_norm_scaled,imgparams);

    meta.ev_user()->emplace_back(uinfo);

    if (meta.plane() == 2)
      _ctr += 1;
    
    return clusters;
  }
 
}

#endif
