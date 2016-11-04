#ifndef __HIPCLUSTER_CXX__
#define __HIPCLUSTER_CXX__

#include "HIPCluster.h"

namespace larocv {

  /// Global larocv::HIPClusterFactory to register AlgoFactory
  static HIPClusterFactory __global_HIPClusterFactory__;

  void HIPCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _min_hip_cluster_size = pset.get<int>("MinHIPClusterSize",20);
    _min_mip_cluster_size = pset.get<int>("MinMIPClusterSize",20);

    _mip_thresh_v    = pset.get<std::vector<int> >("MIPLevels",{10,10,10});
    _hip_thresh_v    = pset.get<std::vector<int> >("HIPLevels",{55,55,50});

    if (_mip_thresh_v.size() != 3) throw larbys("Provided MIP level is not size 3 (1 values per plane)");
    if (_hip_thresh_v.size() != 3) throw larbys("Provided MIP level is not size 3 (1 values per plane)");
    
    _dilation_size = pset.get<int>("DilationSize",2);
    _dilation_iter = pset.get<int>("DilationIter",1);
    _blur_size     = pset.get<int>("BlurSize",2);

    set_verbosity(msg::kDEBUG);
  }

  larocv::Cluster2DArray_t HIPCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
						const ::cv::Mat& img,
						larocv::ImageMeta& meta,
						larocv::ROI& roi)
  {

    
    ::cv::Mat sb_img;
    
    //Dilate
    auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(_dilation_size,_dilation_size));
    ::cv::dilate(img,sb_img,kernel,::cv::Point(-1,-1),_dilation_iter);
    
    //Blur
    ::cv::blur(sb_img,sb_img,::cv::Size(_blur_size,_blur_size));

    GEO2D_ContourArray_t mip_ctor_v;
    GEO2D_ContourArray_t hip_ctor_v;
    GEO2D_ContourArray_t all_ctor_v;
    
    int MIP_LEVEL = _mip_thresh_v.at(meta.plane());
    int HIP_LEVEL = _hip_thresh_v.at(meta.plane());

    LAROCV_DEBUG() << "Plane : " << meta.plane() << "... MIP level: " << MIP_LEVEL << "... HIP level: " << HIP_LEVEL << std::endl;
    
    //Threshold the input image to certain ADC value, this is our MIP
    ::cv::Mat mip_thresh_m;
    inRange(sb_img, MIP_LEVEL, HIP_LEVEL, mip_thresh_m);
    std::stringstream ss0;
    ss0 << "mip_thresh_m_"<<meta.plane()<<".png";
    cv::imwrite(ss0.str().c_str(),mip_thresh_m);
    
    //Threshold the input image to HIP ADC value, this is our HIP
    ::cv::Mat hip_thresh_m;
    threshold(sb_img, hip_thresh_m,HIP_LEVEL,255,0);
    std::stringstream ss1;
    ss1 << "hip_thresh_m_"<<meta.plane()<<".png";
    cv::imwrite(ss1.str().c_str(),hip_thresh_m);
    
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

    // // get the non zero points of the mip
    // std::vector<geo2d::Vector<int> > all_locations;
    // ::cv::findNonZero(mip_thresh_m, all_locations); 
    
    //  for( const auto& loc: all_locations ) {
    //    for( size_t i = 0; i < hip_ctor_v.size(); i++ ) {
    // 	 if ( ::cv::pointPolygonTest(hip_ctor_v[i],loc,false) < 0 )  // check if point in HIP contour
    // 	   continue;
	 
    // 	 //Something here. Zero out this pixel.
    // 	 mip_thresh_m.at<uchar>(loc.y, loc.x) = 0;
    //    }
    //  }
     
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
     
     uint idx=-1;

     std::vector<size_t> mip_idx_v;
     std::vector<size_t> hip_idx_v;
       
     for (auto& mip_ctor : mip_ctor_v) {
       all_ctor_v.emplace_back(mip_ctor);
       idx++;
       mip_idx_v.push_back(idx);
     }
     for (auto& hip_ctor : hip_ctor_v) {
       all_ctor_v.emplace_back(hip_ctor);
       idx++;
       hip_idx_v.push_back(idx);
     }
          
     LAROCV_DEBUG() << "Plane : " << meta.plane()
		    << " Found MIPs : " << mip_ctor_v.size()
		    << " HIPs : " << hip_ctor_v.size()
		    << " Summed : " << all_ctor_v.size() << "\n";

     auto& hip_data       = AlgoData<data::HIPClusterData>();
     auto& hip_plane_data = hip_data._plane_data_v[meta.plane()];
     
     hip_plane_data._mip_idx_v = mip_idx_v;
     hip_plane_data._hip_idx_v = hip_idx_v;
     
     Cluster2DArray_t oclusters;
     oclusters.resize(all_ctor_v.size());
     
     for(unsigned ik=0;ik<oclusters.size();++ik)

       oclusters[ik]._contour = all_ctor_v[ik];
     
     return oclusters;
     
  }

}
#endif
