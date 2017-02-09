#ifndef __SUPERCLUSTERER_CXX__
#define __SUPERCLUSTERER_CXX__

#include "SuperClusterer.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LArOpenCV/Core/larbys.h"

namespace larocv {

  SuperClusterer::SuperClusterer()
    : laropencv_base("SuperClusterer")
  {
    set_verbosity(msg::kNORMAL);
    _dilation_size = -1; //2 if used by default
    _dilation_iter = -1; //1 if used by default
    _blur_size     = -1; //2 if used by default
    _pi_threshold  = -1; //10 if used by default
  }

  void
  SuperClusterer::PrintConfig() const
  {
    std::stringstream ss;
    ss << "Configuration parameter dump..." << std::endl
       << "    _dilation_size  = " << _dilation_size << std::endl
       << "    _dilation_iter  = " << _dilation_iter << std::endl
       << "    _blur_size      = " << _blur_size      << std::endl
       << "    _pi_threshold   = " << _pi_threshold   << std::endl
       << std::endl;
    LAROCV_INFO() << ss.str();
  }

  void SuperClusterer::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    _dilation_size = pset.get<int>("DilationSize",2);
    _dilation_iter = pset.get<int>("DilationIter",1);
    _blur_size     = pset.get<int>("BlurSize",2);
    _pi_threshold = pset.get<short>("PIThreshold",10);
  }

  void SuperClusterer::CreateSuperCluster(const ::cv::Mat& img, GEO2D_ContourArray_t& super_cluster_v) const
  {
    LAROCV_DEBUG() << std::endl;
    //
    // Analyze fraction of pixels clustered
    // 0) prepare image
    // 1) find a contour that contains the subject 2D vertex, and find allcontained non-zero pixels inside
    // 2) per particle cluster count # of pixels from 0) that is contained inside

    // Prepare image for analysis per vertex
    // Threshold
    ::cv::Mat thresh_img;
    if(_pi_threshold<=0)
      thresh_img = img.clone();
    else{
      LAROCV_DEBUG() << "Thresholding to pi threshold: " << _pi_threshold << std::endl;
      ::cv::threshold(img, thresh_img, _pi_threshold, 255, 0);//CV_THRESH_BINARY);
    }

    // Using dilate/blur/threshold for super cluster
    ::cv::Mat blur_img;
    if(_blur_size<=0) 
      blur_img = thresh_img;
    else {
      auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(_dilation_size,_dilation_size));
      ::cv::dilate(thresh_img,blur_img,kernel,::cv::Point(-1,-1),_dilation_iter);
      ::cv::blur(blur_img,blur_img,::cv::Size(_blur_size,_blur_size));
    }

    super_cluster_v.clear();
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    ::cv::findContours(blur_img, super_cluster_v, cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    LAROCV_INFO() << "Created " << super_cluster_v.size()
		  << " super-set contours from image (rows,cols) = (" << blur_img.rows << "," << blur_img.cols << ")" << std::endl;
    if(this->logger().level() == msg::kDEBUG) {
      for(size_t i=0; i<super_cluster_v.size(); ++i)
	LAROCV_DEBUG() << "    Super contour " << i
		       << " ... " << super_cluster_v[i].size() << " contour points" << std::endl;
    }
  }

}

#endif
