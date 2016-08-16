#ifndef __SBCLUSTER_CXX__
#define __SBCLUSTER_CXX__

#include "SBCluster.h"

namespace larocv {


  void SBCluster::_Configure_(const ::fcllite::PSet &pset)
  {

    _dilation_size = pset.get<int> ("DilationSize");
    _dilation_iter = pset.get<int> ("DilationIterations");
    
    _blur_size     = pset.get<int> ("BlurSize");
    
    _thresh        = pset.get<float> ("Thresh");
    _thresh_maxval = pset.get<float> ("ThreshMaxVal");

  }

  larocv::Cluster2DArray_t SBCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
						const ::cv::Mat& img,
						larocv::ImageMeta& meta,
						larocv::ROI& roi)
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

    //Fill some cluster parameters 
    Cluster2DArray_t result_v; result_v.reserve(ctor_v.size());

    ::larocv::Cluster2D new_clus ;

    LAROCV_DEBUG((*this)) << "Found " << ctor_v.size() << " contours\n";
    
    for(size_t j = 0; j < ctor_v.size(); ++j){

      cv::RotatedRect rect0 = ::cv::minAreaRect(cv::Mat(ctor_v[j]));
      cv::Point2f vertices[4];
      rect0.points(vertices);
      auto rect = rect0.size;
      std::vector<cv::Point2f> rectangle = { vertices[0], vertices[1],vertices[2],vertices[3] };
      std::swap(new_clus._minAreaRect,rectangle);

      new_clus._area = ::cv::contourArea(ctor_v[j]) ;
      new_clus._perimeter = ::cv::arcLength(ctor_v[j],1);
      new_clus._length    = rect.height > rect.width ? rect.height : rect.width;
      new_clus._width     = rect.height > rect.width ? rect.width  : rect.height;
      new_clus._numHits   = 0 ;
      new_clus._sumCharge = 0 ;
      
      std::swap(new_clus._contour,ctor_v[j]);
       
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
    
    /// Debug mode is off, return the result
    if (!meta.debug()) return result_v;


    //***************************************************
    //***Debug Mode
    
    std::stringstream ss_x,ss_y;

    ::larlite::user_info uinfo{};

    for(size_t i = 0; i < result_v.size(); ++i){
      const auto& imgclus = result_v[i];

      ss_x  << Name() << "_" << meta.plane() << "_" << i << "_x";
      ss_y  << Name() << "_" << meta.plane() << "_" << i << "_y";

      for(const auto& point : imgclus._contour) {
	double x = meta.XtoTimeTick(point.x);
	double y = meta.YtoWire(point.y);
	
	uinfo.append(ss_x.str(),x);
	uinfo.append(ss_y.str(),y);
      }

      ss_x.str(std::string());
      ss_x.clear();

      ss_y.str(std::string());
      ss_y.clear();
	
    }    
    meta.ev_user()->emplace_back(uinfo);

    return result_v;
  }
 
}

#endif
