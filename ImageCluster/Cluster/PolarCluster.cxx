#ifndef __POLARCLUSTER_CXX__
#define __POLARCLUSTER_CXX__

#include "PolarCluster.h"
#include <stdio.h>

namespace larocv {


  void PolarCluster::_Configure_(const ::fcllite::PSet &pset)
  {

    _ctr = 0;

    _dilation_size = pset.get<int>("DilationSize");
    _dilation_iter = pset.get<int>("DilationIterations");
    
    _blur_size_r = pset.get<int>("BlurSizeR");
    _blur_size_t = pset.get<int>("BlurSizeT");
    
    _thresh        = pset.get<float>("Thresh");
    _thresh_maxval = pset.get<float>("ThreshMaxVal");

  }

  larocv::Cluster2DArray_t PolarCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
						   const ::cv::Mat& img,
						   larocv::ImageMeta& meta,
						   larocv::ROI& roi)
  {

    if ( clusters.size() )
      throw larbys("This algo can only be executed first in algo chain!");
					
    ::cv::Mat polarimg, sb_img;

    //get the vertex in the image
    auto pi0st = roi.roivtx_in_image(meta);

    auto roib1 = roi.roibounds_in_image(meta,0);
    auto roib2 = roi.roibounds_in_image(meta,2);

    auto radx = std::fabs(roib1.x - roib2.x);
    auto rady = std::fabs(roib1.y - roib2.y);

    //std::cout << "corners : (" << roib1.x << ", " << roib1.y << " )" << std::endl;
    //std::cout << "corners : (" << roib2.x << ", " << roib2.y << " )" << std::endl;
    //std::cout << "IMG x width : " << radx << " y width : " << rady << std::endl;
    //std::cout << "pi0 coord : " << pi0st.x << ", " << pi0st.y << std::endl;

    double rad = radx;

    //std::ostringstream imgname;
    //imgname << "img_" << _ctr << ".jpg";

    std::vector<int> imgparams;
    imgparams.push_back( CV_IMWRITE_JPEG_QUALITY );
    imgparams.push_back( 100 );

    std::vector<::cv::Point> all_locations;
    ::cv::findNonZero(img, all_locations);
    //std::cout << "nonzero : " << all_locations.size() << std::endl;
    
    //::cv::imwrite(imgname.str(),img,imgparams);

    ::cv::linearPolar(img,        //input
		      img,   //output
		      ::cv::Point2f(pi0st.x,pi0st.y), //center point
		      rad, //radius
		      ::cv::WARP_FILL_OUTLIERS); //seems like it has to set

    //::cv::findNonZero(img, all_locations);
    //std::cout << "nonzero : " << all_locations.size() << std::endl;
    //std::cout << std::endl;
    
    //std::ostringstream polarimgname;
    //polarimgname << "img_polar_" << _ctr << ".jpg";
    
    _ctr += 1;

    //Blur
    ::cv::blur(img,img,
	       ::cv::Size(_blur_size_r,_blur_size_t));

    //Dilate
    auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,
					      ::cv::Size(_dilation_size,_dilation_size));
    ::cv::dilate(img,img,
		 kernel,::cv::Point(-1,-1),_dilation_iter);
    


    
    //::cv::imwrite(polarimgname.str(),img,imgparams);
    
    //Threshold
    auto t_value = ::cv::threshold(img,
				   img,
				   _thresh,
				   _thresh_maxval,
				   CV_THRESH_BINARY); //return type is "threshold value?"

    (void) t_value;



    //Contour finding
    ContourArray_t ctor_v;    
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    ctor_v.clear(); cv_hierarchy_v.clear();
    
    ::cv::findContours(img,ctor_v,cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    
    //Fill some cluster parameters 
    Cluster2DArray_t result_v;
    result_v.reserve(ctor_v.size());
    
    ::larocv::Cluster2D new_clus ;

    LAROCV_DEBUG((*this)) << "Found " << ctor_v.size() << " contours\n";

    float rows = img.rows;
    float cols = img.cols;
      
    for(size_t j = 0; j < ctor_v.size(); ++j){
      
      Contour_t contour = ctor_v[j];

      for (auto& pt : contour) {
	//up case to floating point
	float r = pt.x;
	float t = pt.y;
	
	r = (r / cols) * rad;
	t = (t / rows) * 360.0 * 3.14159 / 180.0;
	
	pt.x = (int) r * std::cos(t) + pi0st.x;
	pt.y = (int) r * std::sin(t) + pi0st.y;
      }
      
      std::swap(new_clus._contour,contour);
       
      result_v.emplace_back(new_clus);
    }
    
    if ( meta.debug() ) {

      std::stringstream ss1, ss2;

      ::larlite::user_info uinfo{};
      ss1 << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_clusters";
      uinfo.store("ID",ss1.str());

      ss1.str(std::string());
      ss1.clear();

      uinfo.store("NClusters",(int)result_v.size());

      LAROCV_DEBUG((*this)) << "Writing debug information for " << clusters.size() << "\n";
    
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
