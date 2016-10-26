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

    _theta_hi = 10;
    _theta_lo = 10;
    
    auto const vtx_algo_name = pset.get<std::string>("Refine2DVertexAlgo");
    _vtx_algo_id = this->ID(vtx_algo_name);
    
    _use_theta_half_angle = true;
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

    Cluster2DArray_t result_v;

    bool use_half_angle = (ref_xs_v.size() > 2) && _use_theta_half_angle;
    
    for(int xs_pt_idx=0; xs_pt_idx<ref_xs_v.size(); ++xs_pt_idx) {

      float angle,theta_lo,theta_hi;
	
      if (use_half_angle) {
	int idx0  = xs_pt_idx-1 >= 0 ? (xs_pt_idx-1)%ref_xs_v.size() : xs_pt_idx-1+ref_xs_v.size();
	int idx1  = xs_pt_idx;
	int idx2  = (xs_pt_idx+1)%ref_xs_v.size();
	LAROCV_DEBUG() << "idx0 : " << idx0 << "... idx1: " << idx1 << "... idx2: " << idx2 << std::endl;
	      
	auto const& xs0 = ref_xs_v[idx0];
	auto const& xs1 = ref_xs_v[idx1];
	auto const& xs2 = ref_xs_v[idx2];
      
	LAROCV_DEBUG() << "Inspecting XS0 @ " << xs0 << " XS1 @ " << xs1 << " XS2 @ " << xs2 << std::endl;

	float angle0    = geo2d::angle(ref_vtx,xs0);
	float angle1    = geo2d::angle(ref_vtx,xs1);
	float angle2    = geo2d::angle(ref_vtx,xs2);

	LAROCV_DEBUG() << "Angles are 0: " << angle0 << "... 1: " << angle1 << "... 2: " << angle2 << std::endl;

	if ( angle0 < 0 ) angle0+=360;
	if ( angle1 < 0 ) angle1+=360;
	if ( angle2 < 0 ) angle2+=360;
      
	float dangle10 = std::abs(angle1-angle0)/2.0;
	float dangle21 = std::abs(angle2-angle1)/2.0;
      
	angle = angle1 < 0 ? angle1+180-360. : angle1+180;

	LAROCV_DEBUG() << "Computed angle: " << angle << "... dangle10: " << dangle10 << "... dangle21: " << dangle21 << std::endl;

	theta_lo = dangle10;
	theta_hi = dangle21;
	
      } else { 
      
	auto const& xs = ref_xs_v[xs_pt_idx];
	
	LAROCV_DEBUG() << "Inspecting XS @ " << xs << std::endl;
	
	angle = geo2d::angle(ref_vtx,xs);
	angle += 180.;

	theta_lo = _theta_lo;
	theta_hi = _theta_hi;
      
      }
      
      auto rot = ::cv::getRotationMatrix2D(ref_vtx,angle,1.);

      cv::Mat rot_img;
      cv::warpAffine(img, rot_img, rot, img.size());

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

      row_min = (size_t)((float)(rot_polarimg.rows) * (0.5 - theta_lo/360.)); // was 10/360.
      row_max = (size_t)((float)(rot_polarimg.rows) * (0.5 + theta_hi/360.));
      
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
      
      /*
      //Dilate
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

      /*
      ::cv::findContours(sb_img,polar_ctor_v,cv_hierarchy_v,
			 CV_RETR_EXTERNAL,
			 CV_CHAIN_APPROX_SIMPLE);
      */

      ::cv::findContours(rot_polarimg,polar_ctor_v,cv_hierarchy_v,
			 CV_RETR_EXTERNAL,
			 CV_CHAIN_APPROX_SIMPLE);

      LAROCV_DEBUG() << "Found " << polar_ctor_v.size() << " contours\n";
      // Find one contour that is closest to the vtx
      float min_radius=1e4;
      int   target_idx=-1;
      for(size_t polar_ctor_idx = 0; polar_ctor_idx < polar_ctor_v.size(); ++polar_ctor_idx) {
	auto const& polar_ctor = polar_ctor_v[polar_ctor_idx];
	LAROCV_DEBUG() << "Polar contour idx : " << polar_ctor_idx << " of size " << polar_ctor.size() << std::endl;
	for(auto const& pt : polar_ctor) {
	  float angle = pt.y / (float)(rot_polarimg.rows) * 360. - 180;
	  if(angle < -5 || angle > 5) continue;
	  if(pt.x > min_radius) continue;
	  min_radius = pt.x;
	  target_idx = polar_ctor_idx;
	}
	LAROCV_DEBUG() << "min_radius : " << min_radius << std::endl;
      }
      if(target_idx < 0) {
	LAROCV_DEBUG() << "No relevant contour find for this xs point" << std::endl;
	continue;
      }
      
      auto const& polar_contour = polar_ctor_v[target_idx];
      LAROCV_DEBUG() << "Chose polar contour at index : " << target_idx << " of size " << polar_contour.size() << std::endl;
      
      auto& contour_v = data._ctor_vv[meta.plane()];
      
      float rows = rot_polarimg.rows;
      float cols = rot_polarimg.cols;

      // auto const& ctor = polar_contour;
      // std::vector<geo2d::Vector<int> > all_pts_v;
      // std::vector<geo2d::Vector<int> > inside_pts_v;

      // cv::findNonZero(rot_polarimg, all_pts_v);
      // inside_pts_v.reserve(all_pts_v.size());
      
      // for ( auto & pt : all_pts_v) {
      // 	if( pointPolygonTest(ctor, pt, false) < 0 ) continue;

      // 	float r = pt.x;
      // 	float t = pt.y;

      // 	r = (r / cols) * 1000;
      // 	t = ((t / rows) * 360.0 + angle) * M_PI / 180.0;
	
      // 	pt.x = (float) r * std::cos(t) + ref_vtx.x;
      // 	pt.y = (float) r * std::sin(t) + ref_vtx.y;

      // 	pt.x = (int)(pt.x + 0.5);
      // 	pt.y = (int)(pt.y + 0.5);

      // 	inside_pts_v.emplace_back(std::move(pt));
      // }

      // LAROCV_DEBUG() << "inside_pts_v.size() " << inside_pts_v.size() << "\n";
      // cv::Mat m1 = cv::Mat(img.rows,img.cols, CV_8UC1, cvScalar(0.));
      // for (auto& pt : inside_pts_v)
      // 	m1.at<unsigned char>((size_t)pt.y,(size_t)pt.x) = (unsigned char)255;

      // cv::imwrite("poop1.png",m1);
      
      // ContourArray_t actor_v;
      // std::vector<::cv::Vec4i> cv_hierarchy_v1;
      // actor_v.clear(); cv_hierarchy_v1.clear();
      // ::cv::findContours(m1,actor_v,
      // 			 cv_hierarchy_v1,
      // 			 CV_RETR_EXTERNAL,
      // 			 CV_CHAIN_APPROX_SIMPLE);


      // LAROCV_DEBUG() << "ACTOR_V size is : " << actor_v.size() << "\n";
      // Cluster2D res_contour;
      // geo2d::VectorArray<float> contour;
      // for (auto& pt : actor_v[0]) contour.emplace_back(pt.x,pt.y);
      // res_contour._contour=actor_v[0];
      
      Cluster2D res_contour;
      res_contour._contour.resize(polar_contour.size());
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

      	res_contour._contour[pt_idx].x = (int)(pt.x + 0.5);
      	res_contour._contour[pt_idx].y = (int)(pt.y + 0.5);
      }
      
      result_v.emplace_back(std::move(res_contour));
      contour_v.emplace_back(std::move(contour));
    }

    return result_v;
  }
 
}

#endif
