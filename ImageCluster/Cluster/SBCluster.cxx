#ifndef __SBCLUSTER_CXX__
#define __SBCLUSTER_CXX__

#include "SBCluster.h"

namespace larocv {


  void SBCluster::_Configure_(const ::fcllite::PSet &pset)
  {

    _dilation_size = pset.get<int>("DilationSize");
    _dilation_iter = pset.get<int>("DilationIterations");
    
    _blur_size     = pset.get<int>("BlurSize");
    
    _thresh        = pset.get<float>("Thresh");
    _thresh_maxval = pset.get<float>("ThreshMaxVal");

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
    (void) t_value;

    //Contour finding
    ContourArray_t ctor_v;    
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    ctor_v.clear(); cv_hierarchy_v.clear();
    
    ::cv::findContours(sb_img,ctor_v,cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);

    //Fill some cluster parameters 
    Cluster2DArray_t result_v;
    result_v.reserve(ctor_v.size());

    ::larocv::Cluster2D new_clus ;

    LAROCV_DEBUG() << "Found " << ctor_v.size() << " contours\n";
    
    for(size_t j = 0; j < ctor_v.size(); ++j){

      auto& contour = ctor_v[j];
      std::swap(new_clus._contour,contour);
      result_v.emplace_back(new_clus); //this makes a copy
      
    }
    
    if ( meta.debug() ) {

      std::stringstream ss1, ss2;

      ::larlite::user_info uinfo{};
      ss1 << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_clusters";
      uinfo.store("ID",ss1.str());

      ss1.str(std::string());
      ss1.clear();

      uinfo.store("NClusters",(int)result_v.size());

      LAROCV_DEBUG() << "Writing debug information for " << clusters.size() << "\n";
      
      for(size_t i = 0; i < result_v.size(); ++i){

	const auto& cluster = result_v[i];

	////////////////////////////////////////////
	/////Contour points
      
	ss1  <<  "ClusterID_" << i << "_contour_x";
	ss2  <<  "ClusterID_" << i << "_contour_y";

	for(const auto& point : cluster._contour) {
	  double x = meta.XtoTimeTick(point.x);
	  double y = meta.YtoWire(point.y);
	
	  uinfo.append(ss1.str(),x);
	  uinfo.append(ss2.str(),y);
	}
	
	ss1.str(std::string());
	ss1.clear();
	ss2.str(std::string());
	ss2.clear();
      }

      meta.ev_user()->emplace_back(uinfo);
    }
    
    return result_v;
  }
 
}

#endif
