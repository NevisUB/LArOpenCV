#ifndef __HIPCLUSTER_CXX__
#define __HIPCLUSTER_CXX__

#include "HIPCluster.h"

namespace larocv {

  /// Global larocv::HIPClusterFactory to register AlgoFactory
  static HIPClusterFactory __global_HIPClusterFactory__;

  void HIPCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _ClusterHIPMIP.Configure(pset);
    // _ClusterHIPMIP._mask_hip = true;
    // _ClusterHIPMIP._hip_mask_tolerance = 10;
    
    Register(new data::HIPClusterData);
  }

  larocv::Cluster2DArray_t HIPCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
						 const ::cv::Mat& img,
						 larocv::ImageMeta& meta,
						 larocv::ROI& roi) 
  {
    
    auto hip_mip_p = _ClusterHIPMIP.IsolateHIPMIP(img);

    auto& hip_ctor_v      = hip_mip_p.first;
    auto& mip_ctor_v      = hip_mip_p.second;
    auto& mip_ctor_mask_v = hip_mip_p.second;

    const auto& mip_thresh_mask_m = _ClusterHIPMIP.get_mip_masked_img();
    const auto& hip_thresh_m      = _ClusterHIPMIP.get_hip_thresh_img();
    
    uint idx=-1;

    std::vector<size_t> mip_idx_v;
    std::vector<size_t> hip_idx_v;

    std::vector<GEO2D_Contour_t> all_ctor_v;
    all_ctor_v.reserve(hip_ctor_v.size()+mip_ctor_v.size());
    
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

    auto& hip_data       = AlgoData<data::HIPClusterData>(0);
    auto& hip_plane_data = hip_data._plane_data_v[meta.plane()];
    auto& cluster_arr_v  = hip_plane_data.get_clusters();
      
    std::vector<cv::Point_<int> >  points;
    cv::findNonZero(mip_thresh_mask_m, points);
    
    //for the __masked__ mip contours
    LAROCV_DEBUG() << "Looking at " << mip_ctor_mask_v.size() << " MIP contours" << std::endl;

    for ( auto& mip_ctor : mip_ctor_mask_v ) { 
      LAROCV_DEBUG() << "\tThis contour size: " << mip_ctor.size() << std::endl;
      uint npts = 0;
      uint qsum = 0;      

      //tolerance in pixels away from contour edge
      float mip_tolerance = 1.0;
      mip_tolerance*=-1;

      float avg_x = 0;
      float avg_y = 0;

      std::vector<cv::Point_<float> > px_v;
      px_v.reserve(points.size());
      
      for(const auto& pt : points) { 
	auto dist = cv::pointPolygonTest(mip_ctor,pt,true);
	if ( dist < mip_tolerance) continue;
	float space_px = (float) img.at<uchar>(pt.y,pt.x); 
	// if (space_px < MIP_LEVEL) continue;
	npts += 1;
	qsum += (float) img.at<uchar>(pt.y,pt.x);
	avg_x += (float) pt.x;
	avg_y += (float) pt.y;
	qsum += space_px;
	px_v.emplace_back((float)pt.x,(float)pt.y);

      }
      
      avg_x /= (float) npts;
      avg_y /= (float) npts;
      geo2d::Vector<float> center_pt(avg_x,avg_y);
      
      LAROCV_DEBUG() << "\t\t npts " << npts << "... qsum " << qsum << "... iship " << false << std::endl;
      auto rect = cv::minAreaRect(mip_ctor);

      float angle_ = rect.angle;
      // angle_ += 90.0;
      
      auto height = rect.size.height;
      auto width  = rect.size.width;

      if (width < height) angle_+=90;
      
      auto length_ = height > width ? height : width;
      auto width_  = height > width ? width : height;
      
      larocv::data::Cluster cl;
      cl.set_npx(npts);
      cl.set_qsum(qsum);
      cl.set_iship(false);
      cl.set_center_pt(center_pt);
      cl.set_length(length_);
      cl.set_width(width_);
      cl.set_angle(angle_);
      cl.set_px(px_v);
      // cl.set_pixel(pixel_v);
      // cl.set_mip_pixel(mip_pixel_v);
      cluster_arr_v.emplace_back(std::move(cl));
    }
    
    
    
    //for hip contours
    points.clear();
    cv::findNonZero(hip_thresh_m, points);

    LAROCV_DEBUG() << "Looking at " << hip_ctor_v.size() << " masked HIP contours" << std::endl;
    for ( auto& hip_ctor : hip_ctor_v ) { 

      std::vector<cv::Point_<float> > px_v;
      px_v.reserve(points.size());
      
      LAROCV_DEBUG() << "\tThis contour size: " << hip_ctor.size() << std::endl;
      uint npts = 0;
      uint qsum = 0;

      //tolerance in pixels away from contour edge
      float hip_tolerance = 1.0;
      hip_tolerance*=-1;

      float avg_x = 0;
      float avg_y = 0;
      
      for(const auto& pt : points) { 
	auto dist = cv::pointPolygonTest(hip_ctor,pt,true);
	if ( dist < hip_tolerance) continue;
	float space_px = (float) img.at<uchar>(pt.y,pt.x); 
	// if (space_px < HIP_LEVEL) continue;
	npts += 1;
	qsum += space_px;
	avg_x += (float) pt.x;
	avg_y += (float) pt.y;
	px_v.emplace_back((float)pt.x,(float)pt.y);
	// qsum += (uchar) img.at<uchar>(pt.y,pt.x);
	// pixel_v.push_back((float) img.at<uchar>(pt.y,pt.x));
	// pixel_v.push_back(space_px);
	// hip_pixel_v.push_back(space_px);
      }

      avg_x /= (float) npts;
      avg_y /= (float) npts;
      geo2d::Vector<float> center_pt(avg_x,avg_y);

      auto rect = cv::minAreaRect(hip_ctor);
      float angle_ = rect.angle;
      // angle_ += 90.0;
			    
      auto height = rect.size.height;
      auto width  = rect.size.width;

      if (width < height) angle_+=90;
      
      auto length_ = height > width ? height : width;
      auto width_  = height > width ? width : height;
      
      larocv::data::Cluster cl;
      cl.set_npx(npts);
      cl.set_qsum(qsum);
      cl.set_iship(true);
      cl.set_center_pt(center_pt);
      cl.set_length(length_);
      cl.set_width(width_);
      cl.set_angle(angle_);
      cl.set_px(px_v);
      // cl.set_pixel(pixel_v);
      // cl.set_hip_pixel(hip_pixel_v);
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
