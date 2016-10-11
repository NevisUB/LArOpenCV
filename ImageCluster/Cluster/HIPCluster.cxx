#ifndef __HIPCLUSTER_CXX__
#define __HIPCLUSTER_CXX__

#include "HIPCluster.h"
#include "HIPClusterData.h"

namespace larocv {


  void HIPCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _min_hip_cluster_size = 6;
    _min_mip_cluster_size = 6;
    _mip_thresh = 1;
    _hip_thresh = 255;
  }

  larocv::Cluster2DArray_t HIPCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
						const ::cv::Mat& img,
						larocv::ImageMeta& meta,
						larocv::ROI& roi)
  {
    GEO2D_ContourArray_t mip_ctor_v;
    GEO2D_ContourArray_t hip_ctor_v;
    GEO2D_ContourArray_t all_ctor_v;
    
    int THRESH_LOWER = _mip_thresh;
    int HIP_LEVEL    = _hip_thresh;
    
    //Threshold the input image to certain ADC value, this is our MIP
    ::cv::Mat img_thresh_m;
    threshold(img, img_thresh_m,THRESH_LOWER,255,0);
    
    //Threshold the input image to HIP ADC value, this is our HIP
    ::cv::Mat hip_thresh_m;
    threshold(img, hip_thresh_m,HIP_LEVEL,255,0);

    //Contour finding on the HIP
    std::vector<::cv::Vec4i> hip_cv_hierarchy_v;
    hip_cv_hierarchy_v.clear();
    
    ::cv::findContours(hip_thresh_m,
		       hip_ctor_v,
		       hip_cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    
    //Filter the HIP contours to a minimum size
    int min_hip_size = _min_hip_cluster_size;
    
    //ContourArray_t hip_ctor_v_tmp;
    GEO2D_ContourArray_t hip_ctor_v_tmp;
    hip_ctor_v_tmp.reserve(hip_ctor_v.size());
    
    for (const auto& hip_ctor : hip_ctor_v)
      if ( hip_ctor.size() > min_hip_size)
	hip_ctor_v_tmp.emplace_back(hip_ctor);
    
    //swap them out -- the thresholded hips and all hips
    std::swap(hip_ctor_v,hip_ctor_v_tmp);
    
    //Masking away the hip in the original image
    //find the non zero pixels in the hip contours. Mask them out of the MIP image.

    //clone the current mip image
    ::cv::Mat mip_thresh_m = img_thresh_m.clone();
    
    // get the non zero points of the mip
    std::vector<geo2d::Vector<int> > all_locations;
    ::cv::findNonZero(mip_thresh_m, all_locations); 
    
     for( const auto& loc: all_locations ) {
       for( size_t i = 0; i < hip_ctor_v.size(); i++ ) {
	 if ( ::cv::pointPolygonTest(hip_ctor_v[i],loc,false) < 0 )  // check if point in HIP contour
	   continue;
	 
	 //Something here. Zero out this pixel.
	 mip_thresh_m.at<uchar>(loc.y, loc.x) = 0;
       }
     }

     ///////////////////////////////////////////////
     //run the contour finder on the MIPs
     std::vector<::cv::Vec4i> mip_cv_hierarchy_v;
     mip_cv_hierarchy_v.clear();
     
     ::cv::findContours(mip_thresh_m,
			mip_ctor_v,
			mip_cv_hierarchy_v,
			CV_RETR_EXTERNAL,
			CV_CHAIN_APPROX_SIMPLE);

     ///////////////////////////////////////////////
     //Filter the MIP contours to a minimum size, the ones that are
     int min_mip_size = _min_mip_cluster_size;
     GEO2D_ContourArray_t mip_ctor_v_tmp;
     mip_ctor_v_tmp.reserve(mip_ctor_v.size());
     
     for (const auto& mip_ctor : mip_ctor_v)

       if (mip_ctor.size() > min_mip_size)

	 mip_ctor_v_tmp.emplace_back(mip_ctor);

     //swap them out -- the thresholded mips and all mips
     std::swap(mip_ctor_v,mip_ctor_v_tmp);
     
     //unify the contours into a single array
     for (auto& mip_ctor : mip_ctor_v) {

       if (mip_ctor.size() > 0) 

     	 all_ctor_v.emplace_back(mip_ctor);
       
     }

     for (auto& hip_ctor : hip_ctor_v) {

       if (hip_ctor.size() > 0)

	 all_ctor_v.emplace_back(hip_ctor);
       
     }
     
     
     Cluster2DArray_t oclusters;
     oclusters.resize(all_ctor_v.size());
     
     for(unsigned ik=0;ik<oclusters.size();++ik)

       std::swap(oclusters[ik]._contour,all_ctor_v[ik]);
     
     auto& hip_data = AlgoData<larocv::HIPClusterData>();
     hip_data.set_data(mip_ctor_v,hip_ctor_v,all_ctor_v,meta.plane());
     
     return oclusters;
     
  }

}
#endif
