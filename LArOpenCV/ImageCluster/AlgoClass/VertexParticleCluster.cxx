#ifndef __VERTEXPARTICLECLUSTER_CXX__
#define __VERTEXPARTICLECLUSTER_CXX__

#include "VertexParticleCluster.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#ifdef UNIT_TEST
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/ndarrayobject.h>
#endif

namespace larocv {

  VertexParticleCluster::VertexParticleCluster()
    : laropencv_base("VertexParticleCluster")
  {
    set_verbosity(msg::kNORMAL);
    _prange.set_verbosity(msg::kNORMAL);
    _dilation_size = 2;
    _dilation_iter = 1;
    _blur_size     = 2;
    _theta_hi      = 5;
    _theta_lo      = 5;
    _pi_threshold  = 10;
    _use_theta_half_angle = true;
    _contour_dist_threshold = 5;
    _mask_fraction_radius = -1;
    _mask_min_radius = 3;
    _refine_polar_cluster = true;
    _refine_cartesian_cluster = true;
    _merge_by_mask = false;
    _refine_cartesian_thickness=2;
  }

  void
  VertexParticleCluster::PrintConfig() const
  {
    std::stringstream ss;
    ss << "Configuration parameter dump..." << std::endl
       << "    _dilation_size  = " << _dilation_size << std::endl
       << "    _dilation_iter  = " << _dilation_iter << std::endl
       << "    _blur_size      = " << _blur_size      << std::endl
       << "    _theta_hi       = " << _theta_hi       << std::endl
       << "    _theta_lo       = " << _theta_lo       << std::endl
       << "    _pi_threshold   = " << _pi_threshold   << std::endl
       << "    _use_half_angle = " << _use_theta_half_angle << std::endl
       << "    _mask_fraction_radius     = " << _mask_fraction_radius << std::endl
       << "    _mask_min_radius          = " << _mask_min_radius << std::endl
       << "    _refine_cartesian_cluster = " << _refine_cartesian_cluster << std::endl
       << "    _refine_polar_cluster     = " << _refine_polar_cluster << std::endl
       << "    _contour_dist_threshold   = " << _contour_dist_threshold << std::endl
       << std::endl;
    LAROCV_INFO() << ss.str();
  }

  void VertexParticleCluster::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    _prange.set_verbosity(logger().level());
    //_dilation_size = pset.get<int>("DilationSize",2);
    //_dilation_iter = pset.get<int>("DilationIter",1);
    //_blur_size     = pset.get<int>("BlurSize",2);
    
    _theta_hi = pset.get<int>("ThetaHi",10);
    _theta_lo = pset.get<int>("ThetaLo",10);
    _pi_threshold = pset.get<unsigned short>("PIThreshold",10);
    _use_theta_half_angle       = pset.get<bool>("UseHalfAngle",true);
    _contour_dist_threshold     = pset.get<float>("ContourMinDist",5);
    _refine_polar_cluster       = pset.get<bool>("RefinePolarCluster",true);
    _refine_cartesian_cluster   = pset.get<bool>("RefineCartesianCluster",true);
    _mask_fraction_radius       = pset.get<float>("MaskFractionRadius",-1.);
    _mask_min_radius            = pset.get<float>("MaskMinRadius",3);
    _merge_by_mask              = pset.get<bool>("MergeByMask",true);
    _refine_cartesian_thickness = pset.get<uint>("RefineCartesianThickness",2);
  }

  GEO2D_ContourArray_t
  VertexParticleCluster::CreateParticleCluster(const ::cv::Mat& img,
					       const data::CircleVertex& vtx2d,
					       const GEO2D_Contour_t& super_cluster)
  {
    if(this->logger().level() == ::larocv::msg::kDEBUG) {
      std::stringstream ss0;
      ss0 << "orig_plane_input.png";
      cv::imwrite(std::string(ss0.str()).c_str(), img);
    }
    
    LAROCV_DEBUG() << std::endl;
    _prange.set_verbosity(logger().level());
    
    GEO2D_ContourArray_t res;
    
    // Prepare image for analysis per vertex
    // Threshold
    ::cv::Mat thresh_img;
    LAROCV_DEBUG() << "Thresholding to pi threshold: " << _pi_threshold << std::endl;
    ::cv::threshold(img, thresh_img, _pi_threshold, 255,0); //CV_THRESH_BINARY);

    /*
    // Using dilate/blur/threshold for super cluster
    ::cv::Mat blur_img;
    auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(_dilation_size,_dilation_size));
    ::cv::dilate(thresh_img,blur_img,kernel,::cv::Point(-1,-1),_dilation_iter);
    ::cv::blur(blur_img,blur_img,::cv::Size(_blur_size,_blur_size));
    */
    
    // Create seed clusters
    LAROCV_DEBUG() << "Masking region @ " << vtx2d.center << " rad: " << vtx2d.radius << std::endl;

    geo2d::Circle<float> mask_region(vtx2d.center,vtx2d.radius);

    auto img_circle = MaskImage(thresh_img, mask_region, 0, false);
    
    // Create seed clusters
    _seed_cluster_v = ParticleHypothesis(img_circle,vtx2d);

    if(_seed_cluster_v.empty()) return res;

    // Create contours on masked image (outside seed)
    auto img_rest = MaskImage(thresh_img, mask_region, 0, true);
    
    // Apply further mask to exclude outside super contour
    img_rest = MaskImage(img_rest, super_cluster, 0, false);

    // Find contours outside circle
    _child_cluster_v.clear();
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    ::cv::findContours(img_rest, _child_cluster_v, cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    // Analyze correlation of child & seed clusters
    std::vector<bool> used_seed_v(_seed_cluster_v.size(),false);
    std::vector<bool> used_child_v(_child_cluster_v.size(),false);

    std::vector<std::pair<size_t,size_t> > match_v;
    match_v.reserve(vtx2d.xs_v.size());
    for(size_t xs_idx=0; xs_idx < vtx2d.xs_v.size(); ++xs_idx) {
      auto const& xs_pt = vtx2d.xs_v[xs_idx].pt;

      size_t cand_seed_idx  = kINVALID_SIZE;
      size_t cand_child_idx = kINVALID_SIZE;
      double min_dist = 1.e20;
      double dist;
      for(size_t seed_idx=0; seed_idx<_seed_cluster_v.size(); ++seed_idx) {
	if(used_seed_v[seed_idx]) continue;
	dist = cv::pointPolygonTest(_seed_cluster_v[seed_idx],xs_pt,true);
	if(dist > min_dist) continue;
	if(dist > _contour_dist_threshold) continue;
	min_dist = dist;
	cand_seed_idx = seed_idx;
      }
      if(cand_seed_idx != kINVALID_SIZE) used_seed_v[cand_seed_idx] = true;
      
      min_dist = 1.e20;
      for(size_t child_idx=0; child_idx<_child_cluster_v.size(); ++child_idx) {
	if(used_child_v[child_idx]) continue;
	dist = cv::pointPolygonTest(_child_cluster_v[child_idx],xs_pt,true);
	if(dist > min_dist) continue;
	if(dist > _contour_dist_threshold) continue;
	min_dist = dist;
	cand_child_idx = child_idx;
      }
      if(cand_child_idx != kINVALID_SIZE) used_child_v[cand_child_idx] = true;

      match_v[xs_idx] = std::make_pair(cand_seed_idx,cand_child_idx);
    }

    res.resize(vtx2d.xs_v.size());
    for(size_t xs_idx=0; xs_idx<vtx2d.xs_v.size(); ++xs_idx) {

      auto seed_idx  = match_v[xs_idx].first;
      auto child_idx = match_v[xs_idx].second;
      if(seed_idx == kINVALID_SIZE && child_idx == kINVALID_SIZE) {
	LAROCV_WARNING() << "No seed/child matched for XS index " << xs_idx << std::endl;
	continue;
      }
      if(seed_idx == kINVALID_SIZE) {
	LAROCV_WARNING() << "No seed matched for XS index " << xs_idx << std::endl;
	res[xs_idx] = _child_cluster_v[child_idx];
      }
      else if(child_idx == kINVALID_SIZE) {
	LAROCV_WARNING() << "No child matched for XS index " << xs_idx << std::endl;
	res[xs_idx] = _seed_cluster_v[seed_idx];
      }
      else {
	LAROCV_INFO() << "Child " << child_idx << " + Seed " << seed_idx << " matched for XS index " << xs_idx << std::endl;
	res[xs_idx] = _merge_by_mask ?
	std::move(MergeByMask(_seed_cluster_v[seed_idx],_child_cluster_v[child_idx],thresh_img)) :
	std::move(Merge(_seed_cluster_v[seed_idx],_child_cluster_v[child_idx]));
      }
    }
    
    return res;
  }

  std::vector<GEO2D_Contour_t>
  VertexParticleCluster::CreateParticleCluster(const ::cv::Mat& img,
					       const data::CircleVertex& vtx2d,
					       const data::ParticleCluster& super_particle)
  {
    return CreateParticleCluster(img,vtx2d,super_particle._ctor);
  }


  GEO2D_ContourArray_t
  VertexParticleCluster::ParticleHypothesis(const ::cv::Mat& img,
					    const data::CircleVertex& vtx)
  {
    GEO2D_ContourArray_t result_v;
    
    auto const& ref_vtx  = vtx.center;
    auto const& ref_xs_v = vtx.xs_v;
    auto ref_n_xs = ref_xs_v.size();
    LAROCV_INFO() << "# crossing points = " << ref_n_xs << std::endl;
    if(ref_xs_v.empty()) return result_v;
    
    // Order a list of xs points in angle
    std::map<double,size_t> angle_order_m;
    for(size_t pca_idx=0; pca_idx<ref_n_xs; ++pca_idx){
      auto const& pt = ref_xs_v[pca_idx].pt;
      double angle = geo2d::angle(ref_vtx,pt);
      if(angle<0) angle+=360;
      angle_order_m[angle] = pca_idx;
    }

    std::vector<geo2d::Vector<float> > xs_v;
    xs_v.reserve(ref_n_xs);
    std::vector<double> angle_v;
    angle_v.reserve(ref_n_xs);
    for(auto const& angle_idx : angle_order_m) {
      xs_v.push_back(ref_xs_v[angle_idx.second].pt);
      LAROCV_INFO() << "Crossing " << angle_idx.second << " @ "
		    << xs_v.back() << " ... angle = " << angle_idx.first << std::endl;
      angle_v.push_back(angle_idx.first);
    }
    // Define angle range
    std::vector<double> theta_lo_v; // just width, not absolute angle
    std::vector<double> theta_hi_v; // just width, not absolute angle
    theta_lo_v.resize(ref_n_xs,0);
    theta_hi_v.resize(ref_n_xs,360);

    if(ref_n_xs==1) {
      theta_lo_v[0] = 180;
      theta_hi_v[0] = 180;
    }
    if(ref_n_xs==2) {
      theta_hi_v[0] = (angle_v[1] - angle_v[0])/2.;
      theta_lo_v[0] = 180. - theta_hi_v[0];
      theta_hi_v[1] = theta_lo_v[0];
      theta_lo_v[1] = theta_hi_v[0];
    }else{
      for(size_t xs_idx=0; (xs_idx+1)<xs_v.size(); ++xs_idx)
	theta_hi_v[xs_idx] = (angle_v[xs_idx+1] - angle_v[xs_idx])/2.;
      for(size_t xs_idx=1; xs_idx<xs_v.size(); ++xs_idx)
	theta_lo_v[xs_idx] = theta_hi_v[xs_idx-1];
      // take care of hi of last + lo of first
      theta_lo_v.front() = theta_hi_v.back() = 180. - (angle_v.back() - angle_v.front())/2.;
    }
    if(!_use_theta_half_angle) {
      for(size_t xs_idx=0; xs_idx<xs_v.size(); ++xs_idx) {
	if(theta_hi_v[xs_idx] > _theta_hi) theta_hi_v[xs_idx] = _theta_hi;
	if(theta_lo_v[xs_idx] > _theta_lo) theta_lo_v[xs_idx] = _theta_lo;
      }
    }


    for(int xs_pt_idx=0; xs_pt_idx<xs_v.size(); ++xs_pt_idx) {
      LAROCV_DEBUG() << "xs pt idx: " << xs_pt_idx << std::endl;

      auto ref_vtx_copy = ref_vtx;
      LAROCV_DEBUG() << "ref_vtx_copy: " << ref_vtx_copy << std::endl;
      
      float angle = geo2d::angle(ref_vtx_copy,xs_v[xs_pt_idx]) + 180;
      LAROCV_DEBUG() << "angle: " << angle << std::endl;
      
      cv::Mat rot_img;
      cv::Mat img_copy = img.clone();

      cv::Mat img_padded;
      int padding = sqrt(img.rows*img.rows+img.cols*img.cols)/2;
      LAROCV_DEBUG() << "Diagonal is: " << padding << "... from row: " << img.rows << "... and cols: " << img.cols << std::endl;

      img_padded.create(img_copy.rows + 2*padding, img_copy.cols + 2*padding, img_copy.type());
      img_padded.setTo(cv::Scalar::all(0));
      img_copy.copyTo(img_padded(cv::Rect(padding, padding, img.cols, img.rows)));

      ref_vtx_copy.x+=padding;
      ref_vtx_copy.y+=padding;
      
      LAROCV_DEBUG() << "Reference vertex @ " << ref_vtx_copy << std::endl;
      float theta_lo = theta_lo_v[xs_pt_idx];
      float theta_hi = theta_hi_v[xs_pt_idx];
      LAROCV_DEBUG() << "theta lo: " << theta_lo << "... theta hi: " << theta_hi << std::endl;
	
      auto rot = ::cv::getRotationMatrix2D(ref_vtx_copy,angle,1.);
      cv::Rect bbox = cv::RotatedRect(ref_vtx_copy,img_padded.size(),angle).boundingRect(); 

      LAROCV_DEBUG() << "bbox : " << bbox << "... size " << bbox.size() << std::endl;
      cv::warpAffine(img_padded, rot_img, rot, bbox.size());

      LAROCV_DEBUG() << "rot_img rows: " << rot_img.rows << "... cols: " << rot_img.cols << std::endl;
      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss0,ss1,ss2;
	ss0 << "norm_plane_xs" << xs_pt_idx << ".png";
	ss1 << "pad_plane_xs" << xs_pt_idx << ".png";
	ss2 << "rot_plane_xs" << xs_pt_idx << ".png";
	cv::imwrite(std::string(ss0.str()).c_str(), img);
	cv::imwrite(std::string(ss1.str()).c_str(), img_padded);
	cv::imwrite(std::string(ss2.str()).c_str(), rot_img);
      }
      
      cv::Mat rot_polarimg, sb_img;
      
      // Cluster per xs-point found in Refine2DVertex
      const float max_radius_range = 1.5;
      int max_radius=vtx.radius * max_radius_range;

      LAROCV_DEBUG() << "Set max_radius: " << max_radius << std::endl;
      
      ::cv::threshold(rot_img,rot_img,_pi_threshold,255,CV_THRESH_BINARY);
      
      cv::linearPolar(rot_img,        //input
		      rot_polarimg,   //output
		      ref_vtx_copy,
		      max_radius,
		      cv::WARP_FILL_OUTLIERS); //seems like it has to set

      auto kernel = ::cv::getStructuringElement(cv::MORPH_RECT,
      						cv::Size(20,2));
      ::cv::dilate(rot_polarimg,rot_polarimg,kernel,::cv::Point(-1,-1),1);     

      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss2,ss3;
	ss2 << "thresh_plane_xs" << xs_pt_idx << ".png";
	ss3 << "polar_plane_xs" << xs_pt_idx << ".png";
	cv::imwrite(std::string(ss2.str()).c_str(), rot_img);
	cv::imwrite(std::string(ss3.str()).c_str(), rot_polarimg);
      }

      // mask-out very-near vtx pixels
      float mask_radius = 0;
      if(_mask_fraction_radius >0.) mask_radius = _mask_fraction_radius * max_radius_range * vtx.radius;
      if(_mask_min_radius>0. && mask_radius < _mask_min_radius) mask_radius = _mask_min_radius;
      if(mask_radius>0.) {
	size_t mask_col_max = (int)(mask_radius/(vtx.radius * max_radius_range) * rot_polarimg.cols) + 1;
	for(size_t row=0; row<rot_polarimg.rows; row++) {
	  for(size_t col=0; col<mask_col_max; ++col) {
	    rot_polarimg.at<unsigned char>(row, col) = (unsigned char)0;
	  }
	}
      }

      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss2;
	ss2 << "mask_near_plane_xs" << xs_pt_idx << ".png";
	cv::imwrite(std::string(ss2.str()).c_str(), rot_polarimg);
      }

      
      // mask-out a bit further pixels for angles outside the range
      size_t row_min, row_max;
      
      LAROCV_DEBUG() << "rot_polarimg... rows: " << rot_polarimg.rows << "... cols: " << rot_polarimg.cols << std::endl;
      LAROCV_DEBUG() << "theta_lo: " << theta_lo << "... theta_hi: " << theta_hi << std::endl;

      row_min = (size_t)((float)(rot_polarimg.rows) * (0.5 - theta_lo/360.)); // was 10/360.
      row_max = (size_t)((float)(rot_polarimg.rows) * (0.5 + theta_hi/360.));

      LAROCV_DEBUG() << "row min: " << row_min << "... row max: " << row_max << std::endl;      

      for(size_t row=0; row<=row_min; ++row) {
	for(size_t col=0; col<rot_polarimg.cols; col++) {
	  rot_polarimg.at<unsigned char>(row,col) = (unsigned char)0;
	}
      }
      
      for(size_t row=row_max; row<rot_polarimg.rows; ++row) {
	for(size_t col=0; col<rot_polarimg.cols; col++) {
	  rot_polarimg.at<unsigned char>(row,col) = (unsigned char)0;
	}
      }
      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss4;
	ss4 << "mask_plane_xs" << xs_pt_idx << ".png";
	cv::imwrite(std::string(ss4.str()).c_str(), rot_polarimg);     
      }
      //Contour finding
      ContourArray_t polar_ctor_v;    
      std::vector<::cv::Vec4i> cv_hierarchy_v;
      polar_ctor_v.clear(); cv_hierarchy_v.clear();

      ::cv::findContours(rot_polarimg,
			 polar_ctor_v,
			 cv_hierarchy_v,
			 CV_RETR_EXTERNAL,
			 CV_CHAIN_APPROX_SIMPLE);

      LAROCV_DEBUG() << "Found " << polar_ctor_v.size() << " polar contours" << std::endl;
      // Find one contour that is closest to the vtx
      float min_radius=1e4;
      int   target_idx=-1;
      for(size_t polar_ctor_idx = 0; polar_ctor_idx < polar_ctor_v.size(); ++polar_ctor_idx) {
	auto const& polar_ctor = polar_ctor_v[polar_ctor_idx];
	for(auto const& pt : polar_ctor) {
	  float pt_angle = pt.y / (float)(rot_polarimg.rows) * 360. - 180;
	  if(pt_angle < -5 || pt_angle > 5) continue;
	  if(pt.x > min_radius) continue;
	  min_radius = pt.x;
	  target_idx = polar_ctor_idx;
	}
	LAROCV_DEBUG() << "Polar contour idx : " << polar_ctor_idx << " of size " << polar_ctor.size()
		       << " ... min_radius : " << min_radius << std::endl;
      }
      if(target_idx < 0) {
	LAROCV_DEBUG() << "No relevant contour find for this xs point" << std::endl;
	continue;
      }
      
      LAROCV_DEBUG() << "Chose polar contour at index : " << target_idx << " of size " << polar_ctor_v[target_idx].size() << std::endl;

      _refine_polar_cluster=true;
      if(_refine_polar_cluster) {
	// Refine contour
	::cv::Mat filled_ctor(rot_polarimg.size(),rot_polarimg.type(),CV_8UC1);
	polar_ctor_v[0] = polar_ctor_v[target_idx];
	polar_ctor_v.resize(1);
	::cv::drawContours(filled_ctor, polar_ctor_v, -1, cv::Scalar(255), -1, cv::LINE_8); // fill inside
	polar_ctor_v.clear();
	cv_hierarchy_v.clear();
	::cv::findContours(filled_ctor, polar_ctor_v, cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	if(this->logger().level() == ::larocv::msg::kDEBUG) {
	  auto img_tmp0 = rot_polarimg.clone();
	  img_tmp0.setTo(0);
	  drawContours(img_tmp0, GEO2D_ContourArray_t(1,polar_ctor_v[target_idx]), -1 , cv::Scalar(255),1,8);
	  ::cv::imwrite("refine_polar_ctor_a.png",img_tmp0);
	  ::cv::imwrite("refine_polar_ctor_b.png",filled_ctor);
	}
	
	// pick the maximal area contour
	if(polar_ctor_v.empty()) {
	  LAROCV_CRITICAL() << "Lost contour in polar-refining step?!" << std::endl;	
	  throw larbys();
	}
	target_idx = 0;
	if(polar_ctor_v.size()>1) {
	  double max_ctor_area = 0;
	  for(size_t cand_idx=0; cand_idx<polar_ctor_v.size(); ++cand_idx) {
	    double ctor_area = cv::contourArea(polar_ctor_v[cand_idx], false);
	    if(ctor_area < max_ctor_area) continue;
	    max_ctor_area = ctor_area;
	    target_idx = cand_idx;
	  }
	}
      }

      auto const& polar_contour = polar_ctor_v[target_idx];
      
      float rows = rot_polarimg.rows;
      float cols = rot_polarimg.cols;
      
      GEO2D_Contour_t contour;
      contour.reserve(polar_contour.size());

      ::geo2d::Vector<int> this_pt,last_pt;
      ::geo2d::Vector<float> this_fpt;

      auto xs_pt_angle = ref_n_xs==1 ? 180  : angle_v[xs_pt_idx];
      LAROCV_DEBUG() << "Setting xs point index: " << xs_pt_idx
		     << " angle " << angle_v[xs_pt_idx]
		     << " theta lo " << theta_lo
		     << " theta hi " << theta_hi << std::endl;
      _prange.SetAngle(xs_pt_angle,theta_lo,theta_hi);

      
      for (size_t pt_idx=0; pt_idx<polar_contour.size(); ++pt_idx) {
      	auto const& polar_pt = polar_contour[pt_idx];
	
      	//up case to floating point
      	float r = polar_pt.x;
      	float t = polar_pt.y;
	
      	r = (r / cols) * max_radius;
      	t = ((t / rows) * 360.0 + angle) * M_PI / 180.0;
	
      	this_pt.x = (size_t)((float) r * std::cos(t) + ref_vtx_copy.x + 0.5);
      	this_pt.y = (size_t)((float) r * std::sin(t) + ref_vtx_copy.y + 0.5);
	
	this_pt.x -= padding;
	this_pt.y -= padding;
	
	if(pt_idx == 0 || this_pt.x != last_pt.x || this_pt.y != last_pt.y) {
	  // compute the angle of this contour point, make sure it is within the angular range

	  this_fpt.x = this_pt.x;
	  this_fpt.y = this_pt.y;
	  
	  double this_angle = ::geo2d::angle(ref_vtx,this_fpt);
	  if(!_prange.Inside(this_angle)) {
	    double this_dist = ::geo2d::dist(ref_vtx,this_fpt);
	    this_angle = (_prange.CloserEdge(this_angle) ? _prange.AngleHi() : _prange.AngleLo());
	    this_fpt.x = ref_vtx.x + this_dist * cos( 2 * M_PI * this_angle/360.);
	    this_fpt.y = ref_vtx.y + this_dist * sin( 2 * M_PI * this_angle/360.);
	    this_pt.x = (int)this_fpt.x;
	    this_pt.y = (int)this_fpt.y;
	  }
	  if(pt_idx == 0 || this_pt.x != last_pt.x || this_pt.y != last_pt.y) {
	    contour.push_back(this_pt);
	  }
	}
	last_pt = this_pt;
      }

      if(_refine_cartesian_cluster) {
	// Refine contour
	LAROCV_DEBUG() << "Refining cartesian contour of size: " << contour.size() << std::endl;

	auto masked = MaskImage(img,contour,0,false);
	if (!cv::countNonZero(masked)) {
	  LAROCV_WARNING() << "...masked image is empty, pad contour w/ " << _refine_cartesian_thickness << " px" << std::endl;
	  if (contour.size() == 1) {
	    auto rect=cv::Rect(contour.at(0).x-_refine_cartesian_thickness/2,
			       contour.at(0).y-_refine_cartesian_thickness/2,
			       _refine_cartesian_thickness,_refine_cartesian_thickness);
	    
	    LAROCV_WARNING() << "...this contour of size 1 & creating rectangle " << rect << std::endl;
	    masked = MaskImage(img,rect,0,false);
	  } else { 
	    masked = MaskImage(img,contour,_refine_cartesian_thickness,false);
	  }
	}
	
	if(this->logger().level() == ::larocv::msg::kDEBUG) {
	  auto plane_tmp=0;
	  auto img_tmp0 = img.clone();
	  img_tmp0.setTo(0);
	  drawContours(img_tmp0, GEO2D_ContourArray_t(1,contour), -1 , cv::Scalar(255),1,8);
	  std::stringstream ss_a,ss_b,ss_c;
	  ss_a << "imgid_" << plane_tmp << "_refine_cart_ctor_a.png";
	  ss_b << "imgid_" << plane_tmp << "_refine_cart_ctor_b.png";
	  ss_c << "imgid_" << plane_tmp << "_refine_cart_ctor_c.png";
	  ::cv::imwrite(ss_a.str(),img);
	  ::cv::imwrite(ss_b.str(),img_tmp0);
	  ::cv::imwrite(ss_c.str(),masked);
	  if (contour.size()==1) std::exit(1);
	}
	
	GEO2D_ContourArray_t cartesian_ctor_v;
	cartesian_ctor_v.clear();
	cv_hierarchy_v.clear();
	findContours(masked, cartesian_ctor_v, cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	LAROCV_DEBUG() << "Found " << cartesian_ctor_v.size() << " contours in masked image" << std::endl;
	// pick the maximal area contour
	if(cartesian_ctor_v.empty()) {
	  LAROCV_CRITICAL() << "Lost contour in cartesian-refining step?!" << std::endl;
	  throw larbys();
	}
	size_t target_idx = 0;
	if(cartesian_ctor_v.size()>1) {
	  double max_ctor_area = 0;
	  for(size_t cand_idx=0; cand_idx<cartesian_ctor_v.size(); ++cand_idx) {
	    double ctor_area = cv::contourArea(cartesian_ctor_v[cand_idx], false);
	    if(ctor_area < max_ctor_area) continue;
	    max_ctor_area = ctor_area;
	    target_idx = cand_idx;
	  }
	}	
	contour = cartesian_ctor_v[target_idx];
      }
      result_v.emplace_back(std::move(contour));
    }
    
    return result_v;
  }

}

#endif
