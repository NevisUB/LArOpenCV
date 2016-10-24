#ifndef __VERTEXTRACKCLUSTER_CXX__
#define __VERTEXTRACKCLUSTER_CXX__

#include "VertexTrackCluster.h"
#include "Refine2DVertex.h"

namespace larocv {

  /// Global larocv::VertexTrackClusterFactory to register AlgoFactory
  static VertexTrackClusterFactory __global_VERTEXTRACKClusterFactory__;

  void VertexTrackCluster::_Configure_(const ::fcllite::PSet &pset)
  {

    _dilation_size = pset.get<int>("DilationSize");
    _dilation_iter = pset.get<int>("DilationIterations");
    
    _blur_size_r = pset.get<int>("BlurSizeR");
    _blur_size_t = pset.get<int>("BlurSizeT");
    
    _thresh        = pset.get<float>("Thresh");
    _thresh_maxval = pset.get<float>("ThreshMaxVal");

    _mask_radius = 5.;

    auto const vtx_algo_name = pset.get<std::string>("Refine2DVertexAlgo");
    _vtx_algo_id = this->ID(vtx_algo_name);
    
  }

  larocv::Cluster2DArray_t VertexTrackCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
							 const ::cv::Mat& img,
							 larocv::ImageMeta& meta,
							 larocv::ROI& roi)
  {
    
    auto const& ref_data = AlgoData<larocv::Refine2DVertexData>(_vtx_algo_id);
    auto const& ref_vtx = ref_data._cand_vtx_v[meta.plane()];
    auto const& ref_xs_v = ref_data._cand_xs_vv[meta.plane()];
    auto& data = AlgoData<larocv::VertexTrackClusterData>();

    LAROCV_DEBUG() << "Found " << ref_xs_v.size() << " crossing points (track cluster candidates)" << std::endl;
    
    for(size_t xs_pt_idx=0; xs_pt_idx<ref_xs_v.size(); ++xs_pt_idx) {

      auto const& xs = ref_xs_v[xs_pt_idx];

      LAROCV_DEBUG() << "Inspecting XS @ " << xs << std::endl;
      
      float angle = geo2d::angle(ref_vtx,xs);
      angle += 180.;
      
      auto rot = ::cv::getRotationMatrix2D(ref_vtx,angle,1.);

      cv::Mat thresh_img;
      ::cv::threshold(img, thresh_img, 10,255,CV_THRESH_BINARY);
      
      cv::Rect bbox = cv::RotatedRect(ref_vtx,img.size(),angle).boundingRect();
      //rot.at<double>(0,2) += bbox.width/2.0 - ref_vtx.x;
      //rot.at<double>(1,2) += bbox.height/2.0 - ref_vtx.y;

      cv::Mat rot_img;
      cv::warpAffine(img, rot_img, rot, img.size());//, bbox.size());

      std::stringstream ss1,ss2;
      ss1 << "norm_plane" << meta.plane() << "_xs" << xs_pt_idx << ".png";
      ss2 << "rot_plane" << meta.plane() << "_xs" << xs_pt_idx << ".png";

      cv::imwrite(std::string(ss1.str()).c_str(), img);
      cv::imwrite(std::string(ss2.str()).c_str(), rot_img);

      cv::Mat rot_polarimg, sb_img;
      
      // Cluster per xs-point found in Refine2DVertex
      
      ::cv::linearPolar(rot_img,        //input
			rot_polarimg,   //output
			ref_vtx,
			1000,
			::cv::WARP_FILL_OUTLIERS); //seems like it has to set
      ::cv::threshold(rot_polarimg,rot_polarimg,10,255,CV_THRESH_BINARY);

      std::stringstream ss3;
      ss3 << "polar_plane" << meta.plane() << "_xs" << xs_pt_idx << ".png";
      cv::imwrite(std::string(ss3.str()).c_str(), rot_polarimg);

      /*
      // mask-out very-near vtx pixels
      size_t mask_col_max = _mask_radius/1000. * rot_polarimg.cols + 1;
      for(size_t row=0; row<rot_polarimg.rows; row++) {
	for(size_t col=0; col<mask_col_max; ++col) {
	  rot_polarimg.at<unsigned char>(row, col) = (unsigned char)0;
	}
      }
      */
      // mask-out a bit further pixels for angles outside the range
      size_t row_min, row_max;
      row_min = (size_t)((float)(rot_polarimg.rows) * (0.5 - 10./360.));
      row_max = (size_t)((float)(rot_polarimg.rows) * (0.5 + 10./360.));
      for(size_t row=0; row<=row_min; ++row) {
	for(size_t col=0; col<rot_polarimg.cols; col++) {
	  rot_polarimg.at<unsigned char>(row,col) = (unsigned char)0;
	}
      }
      for(size_t row=row_max; row<=rot_polarimg.rows; ++row) {
	for(size_t col=0; col<rot_polarimg.cols; col++) {
	  rot_polarimg.at<unsigned char>(row,col) = (unsigned char)0;
	}
      }
      
      std::stringstream ss4;
      ss4 << "mask_plane" << meta.plane() << "_xs" << xs_pt_idx << ".png";
      cv::imwrite(std::string(ss4.str()).c_str(), rot_polarimg);
      
    
      //Dilate
      /*
      auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,
						::cv::Size(_dilation_size,_dilation_size));
      ::cv::dilate(rot_polarimg,sb_img,
		   kernel,::cv::Point(-1,-1),_dilation_iter);
      
      //Blur
      ::cv::blur(sb_img,sb_img,
		 ::cv::Size(_blur_size_r,_blur_size_t));
      
      //Threshold
      auto t_value = ::cv::threshold(sb_img,
				     sb_img,
				     _thresh,
				     _thresh_maxval,
				     CV_THRESH_BINARY); //return type is "threshold value?"
      (void) t_value;
      */    
      //Contour finding
      ContourArray_t polar_ctor_v;    
      std::vector<::cv::Vec4i> cv_hierarchy_v;
      polar_ctor_v.clear(); cv_hierarchy_v.clear();
      
      ::cv::findContours(rot_polarimg,polar_ctor_v,cv_hierarchy_v,
			 CV_RETR_EXTERNAL,
			 CV_CHAIN_APPROX_SIMPLE);

      LAROCV_DEBUG() << "Found " << polar_ctor_v.size() << " contours\n";
      // Find one contour that is closest to the vtx
      float min_radius=1e4;
      int   target_idx=-1;
      for(size_t polar_ctor_idx = 0; polar_ctor_idx < polar_ctor_v.size(); ++polar_ctor_idx) {
	auto const& polar_ctor = polar_ctor_v[polar_ctor_idx];
	for(auto const& pt : polar_ctor) {
	  float angle = pt.y / (float)(rot_polarimg.rows) * 360. - 180;
	  if(angle < -5 || angle > 5) continue;
	  if(pt.x > min_radius) continue;
	  min_radius = pt.x;
	  target_idx = polar_ctor_idx;
	}
      }
      if(target_idx < 0) {
	LAROCV_DEBUG() << "No relevant contour find for this xs point" << std::endl;
	continue;
      }
      auto const& polar_contour = polar_ctor_v[target_idx];
      auto& contour_v = data._ctor_vv[meta.plane()];
      
      float rows = rot_polarimg.rows;
      float cols = rot_polarimg.cols;
      
      geo2d::VectorArray<float> contour;
      contour.resize(polar_contour.size());
	
      for (size_t pt_idx=0; pt_idx<polar_contour.size(); ++pt_idx) {
	auto const& polar_pt = polar_contour[pt_idx];
	auto& pt = contour[pt_idx];
	//up case to floating point
	float r = polar_pt.x;
	float t = polar_pt.y;
	
	r = (r / cols) * 1000;
	t = ((t / rows) * 360.0 + angle) * M_PI / 180.0;
	
	pt.x = (float) r * std::cos(t) + ref_vtx.x;
	pt.y = (float) r * std::sin(t) + ref_vtx.y;
      }

      contour_v.emplace_back(std::move(contour));
    }
    Cluster2DArray_t result_v;    
    return result_v;
  }
 
}

#endif
