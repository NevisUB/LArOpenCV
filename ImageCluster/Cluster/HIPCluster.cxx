#ifndef __HIPCLUSTER_CXX__
#define __HIPCLUSTER_CXX__

#include "HIPCluster.h"

namespace larocv {

  /// Global larocv::HIPClusterFactory to register AlgoFactory
  static HIPClusterFactory __global_HIPClusterFactory__;

  void HIPCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _min_hip_cluster_size = pset.get<int>("MinHIPClusterSize",5);
    _min_mip_cluster_size = pset.get<int>("MinMIPClusterSize",20);

    _mip_thresh_v    = pset.get<std::vector<int> >("MIPLevels",{10,10,10});
    _hip_thresh_v    = pset.get<std::vector<int> >("HIPLevels",{55,55,50});

    if (_mip_thresh_v.size() != 3) throw larbys("Provided MIP level is not size 3 (1 values per plane)");
    if (_hip_thresh_v.size() != 3) throw larbys("Provided MIP level is not size 3 (1 values per plane)");
    
    _dilation_size = pset.get<int>("DilationSize",2);
    _dilation_iter = pset.get<int>("DilationIter",1);
    _blur_size     = pset.get<int>("BlurSize",2);

    _mask_hip = pset.get<bool>("MaskHIP",false);
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
    GEO2D_ContourArray_t mip_ctor_mask_v;
    GEO2D_ContourArray_t hip_ctor_v;
    GEO2D_ContourArray_t all_ctor_v;
    
    int MIP_LEVEL = _mip_thresh_v.at(meta.plane());
    int HIP_LEVEL = _hip_thresh_v.at(meta.plane());

    LAROCV_DEBUG() << "Plane : " << meta.plane() << "... MIP level: " << MIP_LEVEL << "... HIP level: " << HIP_LEVEL << std::endl;
    
    //Threshold the input image to certain ADC value, this is our MIP
    ::cv::Mat mip_thresh_m;

    // uint HIP_LEVEL_TMP = 255;
    // inRange(sb_img, MIP_LEVEL, HIP_LEVELT_TMP, mip_thresh_m);
    threshold(sb_img, mip_thresh_m, MIP_LEVEL,255,0);
    
    // std::stringstream ss0;
    // ss0 << "mip_thresh_m_"<<meta.plane()<<".png";
    // cv::imwrite(ss0.str().c_str(),mip_thresh_m);
    
    //Threshold the input image to HIP ADC value, this is our HIP
    ::cv::Mat hip_thresh_m;
    threshold(sb_img, hip_thresh_m, HIP_LEVEL,255,0);

    // std::stringstream ss1;
    // ss1 << "hip_thresh_m_"<<meta.plane()<<".png";
    // cv::imwrite(ss1.str().c_str(),hip_thresh_m);

    ///////////////////////////////////////////////
    //HIP contour finding
    std::vector<::cv::Vec4i> hip_cv_hierarchy_v;
    hip_cv_hierarchy_v.clear();
    
    ::cv::findContours(hip_thresh_m,
		       hip_ctor_v,
		       hip_cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    
    //Filter the HIP contours to a minimum size
    int min_hip_size = _min_hip_cluster_size;
    
    GEO2D_ContourArray_t hip_ctor_v_tmp;
    hip_ctor_v_tmp.reserve(hip_ctor_v.size());
    
    for (const auto& hip_ctor : hip_ctor_v)
      if ( hip_ctor.size() > min_hip_size)
	hip_ctor_v_tmp.emplace_back(hip_ctor);
    
    //swap them out -- the thresholded hips and all hips
    std::swap(hip_ctor_v,hip_ctor_v_tmp);
    
    //find the non zero pixels in the hip contours. Mask them out of the MIP image.
    std::vector<cv::Point_<int> > all_locations;
    ::cv::findNonZero(mip_thresh_m, all_locations); 

    cv::Mat mip_thresh_mask_m = mip_thresh_m.clone();
    
    float hip_mask_tolerance = 3.0;
    hip_mask_tolerance*=-1;
    
     for( const auto& loc: all_locations ) {
       for( size_t i = 0; i < hip_ctor_v.size(); i++ ) {
	 auto dist = cv::pointPolygonTest(hip_ctor_v[i],loc,true);
	 if (dist < hip_mask_tolerance) continue;
	 
    	 //Something here. Zero out this pixel.
    	 mip_thresh_mask_m.at<uchar>(loc.y, loc.x) = 0;
       }
     }

    // std::stringstream ss2;
    // ss2 << "mip_thresh_mask_m_"<<meta.plane()<<".png";
    // cv::imwrite(ss2.str().c_str(),mip_thresh_mask_m);
     
    ///////////////////////////////////////////////
    //MIP contour finding
    std::vector<::cv::Vec4i> mip_cv_hierarchy_v;
    mip_cv_hierarchy_v.clear();
    
    ::cv::findContours(mip_thresh_m,
		       mip_ctor_v,
		       mip_cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);


    ///////////////////////////////////////////////
    //run the contour finder on the masked MIPs
    std::vector<::cv::Vec4i> mip_cv_hierarchy_mask_v;
    mip_cv_hierarchy_mask_v.clear();
    
    ::cv::findContours(mip_thresh_mask_m,
		       mip_ctor_mask_v,
		       mip_cv_hierarchy_mask_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);

    ///////////////////////////////////////////////
    //Filter the MIP contours to a minimum size, the ones that are
    int min_mip_size = _min_mip_cluster_size;

    GEO2D_ContourArray_t mip_ctor_v_tmp;
    mip_ctor_v_tmp.reserve(mip_ctor_v.size());

    GEO2D_ContourArray_t mip_ctor_mask_v_tmp;
    mip_ctor_mask_v_tmp.reserve(mip_ctor_mask_v.size());
    
    for (const auto& mip_ctor : mip_ctor_v)
      if (mip_ctor.size() > min_mip_size)
	mip_ctor_v_tmp.emplace_back(mip_ctor);

    for (const auto& mip_ctor : mip_ctor_mask_v)
      if (mip_ctor.size() > min_mip_size)
	mip_ctor_mask_v_tmp.emplace_back(mip_ctor);

    //swap them out -- the thresholded mips and all mips
    std::swap(mip_ctor_v,mip_ctor_v_tmp);

    //swap them out -- the thresholded masked mips and all masked mips
    std::swap(mip_ctor_mask_v,mip_ctor_mask_v_tmp);
     
    uint idx=-1;

    std::vector<size_t> mip_idx_v;
    std::vector<size_t> hip_idx_v;

    //put the non-masked mip contours in all_ctor_v
    for (auto& mip_ctor : mip_ctor_v) {
      all_ctor_v.emplace_back(mip_ctor);
      idx++;
      mip_idx_v.push_back(idx);
    }
    //out the hip contours in all_ctor_v
    for (auto& hip_ctor : hip_ctor_v) {
      all_ctor_v.emplace_back(hip_ctor);
      idx++;
      hip_idx_v.push_back(idx);
    }
          
    LAROCV_DEBUG() << " Plane: " << meta.plane()
		   << " Found MIPs: " << mip_ctor_v.size()
		   << " HIPs: " << hip_ctor_v.size()
		   << " Summed: " << all_ctor_v.size() << "\n";

    auto& hip_data       = AlgoData<data::HIPClusterData>();
    auto& hip_plane_data = hip_data._plane_data_v[meta.plane()];
    auto& cluster_arr_v  = hip_plane_data.get_clusters();
      
    std::vector<cv::Point_<int> >  points;
    cv::findNonZero(mip_thresh_mask_m, points);
    
    //for the __masked__ mip contours
    //std::cout << "Looking at " << mip_ctor_mask_v.size() << " MIP contours" << std::endl;

    for ( auto& mip_ctor : mip_ctor_mask_v ) { 
      //std::cout << "\tThis contour size: " << mip_ctor.size() << std::endl;
      uint npts = 0;
      uint qsum = 0;      
      std::vector<float> pixel_v;
      pixel_v.clear();
      std::vector<float> mip_pixel_v;
      mip_pixel_v.clear();

      //tolerance in pixels away from contour edge
      float mip_tolerance = 1.0;
      mip_tolerance*=-1;
    
      for(const auto& pt : points) { 
	auto dist = cv::pointPolygonTest(mip_ctor,pt,true);
	if ( dist < mip_tolerance) continue;
	float space_px = (float) img.at<uchar>(pt.y,pt.x); 
	if (space_px < MIP_LEVEL) continue;
	npts += 1;
	qsum += space_px;
	pixel_v.push_back(space_px);
	mip_pixel_v.push_back(space_px);
      }

      //std::cout << "\t\t npts " << npts << "... qsum " << qsum << "... iship " << false << std::endl;
      
      larocv::data::Cluster cl;
      cl.set_npx(npts);
      cl.set_qsum(qsum);
      cl.set_iship(false);
      cl.set_pixel(pixel_v);
      cl.set_mip_pixel(mip_pixel_v);
      cluster_arr_v.emplace_back(std::move(cl));
    }
    
    
    
    //for hip contours
    points.clear();
    cv::findNonZero(hip_thresh_m, points);

    //std::cout << "Looking at " << hip_ctor_v.size() << " masked HIP contours" << std::endl;
    for ( auto& hip_ctor : hip_ctor_v ) { 

      //std::cout << "\tThis contour size: " << hip_ctor.size() << std::endl;
      uint npts = 0;
      uint qsum = 0;
      std::vector<float> pixel_v;
      pixel_v.clear();
      std::vector<float> hip_pixel_v;
      hip_pixel_v.clear();
      //tolerance in pixels away from contour edge
      float hip_tolerance = 1.0;
      hip_tolerance*=-1;
    
      for(const auto& pt : points) { 
	auto dist = cv::pointPolygonTest(hip_ctor,pt,true);
	if ( dist < hip_tolerance) continue;
	float space_px = (float) img.at<uchar>(pt.y,pt.x); 
	if (space_px < HIP_LEVEL) continue;
	npts += 1;
	//qsum += (uchar) img.at<uchar>(pt.y,pt.x);
	//pixel_v.push_back((float) img.at<uchar>(pt.y,pt.x));
	qsum += space_px;
	pixel_v.push_back(space_px);
	hip_pixel_v.push_back(space_px);
      }

      //std::cout << "\t\t npts " << npts << "... qsum " << qsum << "... iship " << false << std::endl;
      
      larocv::data::Cluster cl;
      cl.set_npx(npts);
      cl.set_qsum(qsum);
      cl.set_iship(true);
      cl.set_pixel(pixel_v);
      cl.set_hip_pixel(hip_pixel_v);
      cluster_arr_v.emplace_back(std::move(cl));
    }
    
    hip_plane_data.move_mip_indicies(std::move(mip_idx_v));
    hip_plane_data.move_hip_indicies(std::move(hip_idx_v));

    Cluster2DArray_t oclusters;
    oclusters.resize(all_ctor_v.size());
     
    for(unsigned ik=0;ik<oclusters.size();++ik)

      oclusters[ik]._contour = all_ctor_v[ik];
     
    return oclusters;
     
  }

}
#endif
