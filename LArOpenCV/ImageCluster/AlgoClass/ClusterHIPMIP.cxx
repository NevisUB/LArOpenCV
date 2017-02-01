#ifndef __CLUSTERHIPMIP_CXX__
#define __CLUSTERHIPMIP_CXX__

#include "ClusterHIPMIP.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LArOpenCV/Core/larbys.h"

using larocv::larbys;

namespace larocv {

  ClusterHIPMIP::ClusterHIPMIP() :
    _mip_thresh_mask_m(), // empty cv mat
    _mip_thresh_m(), // empty cv mat
    _hip_thresh_m() //empty cv mat
  {
    _min_hip_cluster_size = 5;
    _min_mip_cluster_size = 20;
    
    _mip_thresh = 10;
    _hip_thresh = 50;
      
    _dilation_size = 2;
    _dilation_iter = 1;
    _blur_size     = 2;

    _mask_hip = false;

    _hip_mask_tolerance = 3;
  }

  void ClusterHIPMIP::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    
    _min_hip_cluster_size = pset.get<int>("MinHIPClusterSize",5);
    _min_mip_cluster_size = pset.get<int>("MinMIPClusterSize",20);

    _mip_thresh = pset.get<int>("MIPLevels",10);
    _hip_thresh = pset.get<int>("HIPLevels",50);

    _dilation_size = pset.get<int>("DilationSize",2);
    _dilation_iter = pset.get<int>("DilationIter",1);
    _blur_size     = pset.get<int>("BlurSize",2);

    _mask_hip = pset.get<bool>("MaskHIP",false);

    _hip_mask_tolerance = pset.get<int>("HIPMaskTolerance",3.0);
    
  }

  std::pair<std::vector<GEO2D_Contour_t>, std::vector<GEO2D_Contour_t> >
  ClusterHIPMIP::IsolateHIPMIP(const ::cv::Mat& img) {
    
    cv::Mat mod_img_m;
    
    //Dilate
    auto kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(_dilation_size,_dilation_size));
    cv::dilate(img,mod_img_m,kernel,cv::Point(-1,-1),_dilation_iter);
    
    //Blur
    cv::blur(mod_img_m,mod_img_m,cv::Size(_blur_size,_blur_size));

    GEO2D_ContourArray_t mip_ctor_v;
    GEO2D_ContourArray_t mip_ctor_mask_v;
    GEO2D_ContourArray_t hip_ctor_v;
    
    int MIP_LEVEL = _mip_thresh;
    int HIP_LEVEL = _hip_thresh;

    LAROCV_DEBUG() << "MIP level: " << MIP_LEVEL << "... HIP level: " << HIP_LEVEL << std::endl;
    
    //Threshold the input image to certain ADC value, this is our MIP
    threshold(mod_img_m, _mip_thresh_m, MIP_LEVEL,255,0);

    //Threshold the input image to HIP ADC value, this is our HIP
    threshold(mod_img_m, _hip_thresh_m, HIP_LEVEL,255,0);
    LAROCV_DEBUG() << "Thresholded HIP" << std::endl;

    //HIP contour finding
    std::vector<cv::Vec4i> hip_cv_hierarchy_v;
    hip_cv_hierarchy_v.clear();
    
    cv::findContours(_hip_thresh_m,
		     hip_ctor_v,
		     hip_cv_hierarchy_v,
		     CV_RETR_EXTERNAL,
		     CV_CHAIN_APPROX_SIMPLE);

    LAROCV_DEBUG() << "Found " << hip_ctor_v.size() << " hip contours"  << std::endl;
    //Filter the HIP contours to a minimum size
    
    GEO2D_ContourArray_t hip_ctor_v_tmp;
    hip_ctor_v_tmp.reserve(hip_ctor_v.size());
    
    for (const auto& hip_ctor : hip_ctor_v)
      if ( hip_ctor.size() > _min_hip_cluster_size)
	hip_ctor_v_tmp.emplace_back(hip_ctor);
    
    //swap the size thresholded hips for all the hips
    std::swap(hip_ctor_v,hip_ctor_v_tmp);

    LAROCV_DEBUG() << "Reduced to " << hip_ctor_v.size() << " hip contours after size cut"  << std::endl;

    //Mask them out of the MIP image.
    LAROCV_DEBUG() << "Masking hip image" << std::endl;
    _mip_thresh_mask_m = MaskImage(_mip_thresh_m,        //input image
				   hip_ctor_v,          //hip contours
				   _hip_mask_tolerance, //make the edges fatter for mask
				   true);                //yes, mask
    
    // mip contour finding
    std::vector<cv::Vec4i> mip_cv_hierarchy_v;
    mip_cv_hierarchy_v.clear();
    
    cv::findContours(_mip_thresh_m,
		     mip_ctor_v,
		     mip_cv_hierarchy_v,
		     CV_RETR_EXTERNAL,
		     CV_CHAIN_APPROX_SIMPLE);


    LAROCV_DEBUG() << "Found " << mip_ctor_v.size() << " MIP contours"<< std::endl;
    
    // masked mip contour finding
    std::vector<cv::Vec4i> mip_cv_hierarchy_mask_v;
    mip_cv_hierarchy_mask_v.clear();

    LAROCV_DEBUG() << "Found " << mip_ctor_v.size() << " masked MIP contours"<< std::endl;
    
    cv::findContours(_mip_thresh_mask_m,
		     mip_ctor_mask_v,
		     mip_cv_hierarchy_mask_v,
		     CV_RETR_EXTERNAL,
		     CV_CHAIN_APPROX_SIMPLE);
    
    //Filter the MIP contours to a minimum size

    // mips
    GEO2D_ContourArray_t mip_ctor_v_tmp;
    mip_ctor_v_tmp.reserve(mip_ctor_v.size());

    // masked mips
    GEO2D_ContourArray_t mip_ctor_mask_v_tmp;
    mip_ctor_mask_v_tmp.reserve(mip_ctor_mask_v.size());
    
    for (const auto& mip_ctor : mip_ctor_v)
      if (mip_ctor.size() > _min_mip_cluster_size)
	mip_ctor_v_tmp.emplace_back(mip_ctor);

    for (const auto& mip_ctor : mip_ctor_mask_v)
      if (mip_ctor.size() > _min_mip_cluster_size)
	mip_ctor_mask_v_tmp.emplace_back(mip_ctor);

    //swap them out -- the thresholded mips and all mips
    std::swap(mip_ctor_v,mip_ctor_v_tmp);

    //swap them out -- the thresholded masked mips and all masked mips
    std::swap(mip_ctor_mask_v,mip_ctor_mask_v_tmp);

    LAROCV_DEBUG() << "Now " << mip_ctor_v.size() << " MIP contours after size cut"<< std::endl;
    LAROCV_DEBUG() << "Now " << mip_ctor_mask_v.size() << " masked MIP contours after size cut" << std::endl;
    
    return _mask_hip ? std::make_pair(hip_ctor_v,mip_ctor_mask_v) : std::make_pair(hip_ctor_v,mip_ctor_v);
  }
  
}

#endif
