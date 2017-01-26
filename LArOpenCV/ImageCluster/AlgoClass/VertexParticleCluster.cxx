#ifndef __VERTEXPARTICLECLUSTER_CXX__
#define __VERTEXPARTICLECLUSTER_CXX__

#include "VertexParticleCluster.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

#ifdef UNIT_TEST
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/ndarrayobject.h>
#endif

namespace larocv {

  VertexParticleCluster::VertexParticleCluster()
  {
    set_verbosity(msg::kNORMAL);
    _dilation_size = 2;
    _dilation_iter = 1;
    _blur_size     = 2;
    _theta_hi      = 5;
    _theta_lo      = 5;
    _pi_threshold  = 10;
    _use_theta_half_angle = true;
    _contour_dist_threshold = 5;
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
       << "    _contour_dist_threshold = " << _contour_dist_threshold << std::endl
       << std::endl;
    LAROCV_NORMAL() << ss.str();
  }

  void VertexParticleCluster::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));

    //_dilation_size = pset.get<int>("DilationSize",2);
    //_dilation_iter = pset.get<int>("DilationIter",1);
    //_blur_size     = pset.get<int>("BlurSize",2);
    
    _theta_hi = pset.get<int>("ThetaHi",20);
    _theta_lo = pset.get<int>("ThetaLo",20);
    _pi_threshold = pset.get<unsigned short>("PIThreshold",10);
    _use_theta_half_angle = true;
    _contour_dist_threshold = pset.get<float>("ContourMinDist",5);
  }

  void
  VertexParticleCluster::CreateSuperCluster(const ::cv::Mat& img)
  {
    LAROCV_DEBUG() << std::endl;
    //
    // Analyze fraction of pixels clustered
    // 0) find a contour that contains the subject 2D vertex, and find allcontained non-zero pixels inside
    // 1) per particle cluster count # of pixels from 0) that is contained inside
    _super_cluster_v.clear();
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    ::cv::findContours(img, _super_cluster_v, cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    LAROCV_INFO() << "Created " << _super_cluster_v.size()
		  << " super-set contours from image (rows,cols) = (" << img.rows << "," << img.cols << ")" << std::endl;
  }

  size_t
  VertexParticleCluster::FindSuperCluster(const ::geo2d::Vector<float>& vtx2d) const
  {
    size_t parent_ctor_id   = kINVALID_SIZE;
    size_t parent_ctor_size = 0;
    double dist2vtx = -1e9;
    LAROCV_DEBUG() << std::endl;
    for(size_t ctor_id=0; ctor_id < _super_cluster_v.size(); ++ctor_id){
      auto const& ctor = _super_cluster_v[ctor_id];
      LAROCV_DEBUG() << "ctor id: " << ctor_id << std::endl;
      auto dist = ::cv::pointPolygonTest(ctor, vtx2d, true);
      LAROCV_DEBUG() << "    dist: " << dist << std::endl;
      if(dist < dist2vtx) continue;
      if(dist2vtx >=0 && parent_ctor_size > ctor.size()) continue;
      parent_ctor_id = ctor_id;
      parent_ctor_size = ctor.size();
      dist2vtx = dist;
      LAROCV_DEBUG() << "    size: " << ctor.size() << std::endl;
    }
    LAROCV_INFO() << "Vertex @ " << vtx2d << " belongs to super cluster id " << parent_ctor_id << std::endl;
    return parent_ctor_id;
  }

  GEO2D_ContourArray_t
  VertexParticleCluster::CreateParticleCluster(const ::cv::Mat& img,
					       const data::CircleVertex& vtx2d)
  {
    LAROCV_DEBUG() << std::endl;
    
    GEO2D_ContourArray_t res;
    
    // Prepare image for analysis per vertex
    ::cv::Mat thresh_img;
    /* 
       // Using dilate/blur/threshold
       auto kernel = ::cv::getStructuringElement(cv::MORPH_ELLIPSE,::cv::Size(_dilation_size,_dilation_size));
       ::cv::dilate(img,thresh_img,kernel,::cv::Point(-1,-1),_dilation_iter);
       ::cv::blur(thresh_img,thresh_img,::cv::Size(_blur_size,_blur_size));
       ::cv::threshold(thresh_img, thresh_img, _pi_threshold, 1, CV_THRESH_BINARY);
    */
    // OR using just threshold
    ::cv::threshold(img, thresh_img, _pi_threshold, 1, CV_THRESH_BINARY);

    // Make super cluster
    CreateSuperCluster(thresh_img);

    // Find if relevant super cluster is found
    auto super_cluster_id = FindSuperCluster(vtx2d.center);
    if(super_cluster_id == kINVALID_SIZE) return res;
    
    // Create seed clusters
    cv::imwrite("original.png", thresh_img);
    geo2d::Circle<float> mask_region(vtx2d.center,vtx2d.radius * 1.5);
    auto img_circle = MaskImage(thresh_img, mask_region, 0, false);
    cv::imwrite("circle_masked.png", img_circle);
    _seed_cluster_v = ParticleHypothesis(img_circle,vtx2d);

    if(_seed_cluster_v.empty()) return res;

    // Create contours on masked image (outside seed)
    auto img_rest = MaskImage(thresh_img, mask_region, 0, true);
    cv::imwrite("circle_outside.png", img_rest);
    
    // Apply further mask to exclude outside super contour
    img_rest = MaskImage(img_rest, _super_cluster_v[super_cluster_id], 0, false);
    cv::imwrite("supercluster_inside.png", img_rest);

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
	dist = Distance(xs_pt,_seed_cluster_v[seed_idx]);
	if(dist > min_dist) continue;
	if(dist > _contour_dist_threshold) continue;
	min_dist = dist;
	cand_seed_idx = seed_idx;
      }
      if(cand_seed_idx != kINVALID_SIZE) used_seed_v[cand_seed_idx] = true;
      
      min_dist = 1.e20;
      for(size_t child_idx=0; child_idx<_child_cluster_v.size(); ++child_idx) {
	if(used_child_v[child_idx]) continue;
	dist = Distance(xs_pt,_child_cluster_v[child_idx]);
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
      if(seed_idx == kINVALID_SIZE && child_idx == kINVALID_SIZE)
	continue;
      if(seed_idx == kINVALID_SIZE)
	res[xs_idx] = _child_cluster_v[child_idx];
      else if(child_idx == kINVALID_SIZE)
	res[xs_idx] = _seed_cluster_v[seed_idx];
      else
	res[xs_idx] = std::move(Merge(_seed_cluster_v[seed_idx],_child_cluster_v[child_idx]));
    }
    
    return res;
  }

  GEO2D_ContourArray_t
  VertexParticleCluster::ParticleHypothesis(const ::cv::Mat& img,
					    const data::CircleVertex& vtx)
  {

    auto const& ref_vtx  = vtx.center;
    auto const& ref_xs_v = vtx.xs_v;

    GEO2D_ContourArray_t result_v;

    bool use_half_angle = (ref_xs_v.size() > 2) && _use_theta_half_angle;
    
    for(int xs_pt_idx=0; xs_pt_idx<ref_xs_v.size(); ++xs_pt_idx) {

      auto ref_vtx_copy = ref_vtx;

      float angle,theta_lo,theta_hi;

      cv::Mat rot_img;
      cv::Mat img_copy = img;

      cv::Mat img_padded;
      int padding = sqrt(img.rows*img.rows+img.cols*img.cols)/2;
      LAROCV_DEBUG() << "Diagonal is: " << padding << "... from row: " << img.rows << "... and cols: " << img.cols << std::endl;

      img_padded.create(img_copy.rows + 2*padding, img_copy.cols + 2*padding, img_copy.type());
      img_padded.setTo(cv::Scalar::all(0));
      img_copy.copyTo(img_padded(cv::Rect(padding, padding, img.cols, img.rows)));

      ref_vtx_copy.x+=padding;
      ref_vtx_copy.y+=padding;
      
      LAROCV_DEBUG() << "Reference vertex @ " << ref_vtx_copy << std::endl;
      
      if (use_half_angle) {
	int idx0  = xs_pt_idx-1 >= 0 ? (xs_pt_idx-1)%ref_xs_v.size() : xs_pt_idx-1+ref_xs_v.size();
	int idx1  = xs_pt_idx;
	int idx2  = (xs_pt_idx+1)%ref_xs_v.size();
	
	LAROCV_DEBUG() << "idx0 : " << idx0 << "... idx1: " << idx1 << "... idx2: " << idx2 << std::endl;

	auto xs0 = ref_xs_v[idx0].pt; xs0.x+=padding; xs0.y+=padding;
	auto xs1 = ref_xs_v[idx1].pt; xs1.x+=padding; xs1.y+=padding;
	auto xs2 = ref_xs_v[idx2].pt; xs2.x+=padding; xs2.y+=padding;
	
	LAROCV_DEBUG() << "Inspecting XS0 @ " << xs0 << " XS1 @ " << xs1 << " XS2 @ " << xs2 << std::endl;

	float angle0  = geo2d::angle(ref_vtx_copy,xs0);
	float angle1  = geo2d::angle(ref_vtx_copy,xs1);
	float angle2  = geo2d::angle(ref_vtx_copy,xs2);

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
      
	auto xs = ref_xs_v[xs_pt_idx].pt; xs.x+=padding; xs.y+=padding;
	
	LAROCV_DEBUG() << "Inspecting XS @ " << xs << std::endl;
	
	angle = geo2d::angle(ref_vtx_copy,xs);
	angle += 180.;

	theta_lo = _theta_lo;
	theta_hi = _theta_hi;
      
      }
      
      auto rot = ::cv::getRotationMatrix2D(ref_vtx_copy,angle,1.);
      cv::Rect bbox = cv::RotatedRect(ref_vtx_copy,img_padded.size(),angle).boundingRect(); 
      LAROCV_DEBUG() << "bbox : " << bbox << "... size " << bbox.size() << std::endl;
      
      cv::warpAffine(img_padded, rot_img, rot, bbox.size());

      LAROCV_DEBUG() << "rot_img rows: " << rot_img.rows << "... cols: " << rot_img.cols << std::endl;

      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss1,ss2;
	ss1 << "norm_plane_xs" << xs_pt_idx << ".png";
	ss2 << "rot_plane_xs" << xs_pt_idx << ".png";
	cv::imwrite(std::string(ss1.str()).c_str(), img_padded);
	cv::imwrite(std::string(ss2.str()).c_str(), rot_img);
      }

      cv::Mat rot_polarimg, sb_img;
      
      // Cluster per xs-point found in Refine2DVertex

      int max_radius=1000;

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
	std::stringstream ss3;
	ss3 << "polar_plane_xs" << xs_pt_idx << ".png";
	cv::imwrite(std::string(ss3.str()).c_str(), rot_polarimg);
      }

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
      
      float rows = rot_polarimg.rows;
      float cols = rot_polarimg.cols;
      
      //cv::Mat polar_ctor_mat = cv::Mat(img_padded.rows,img_padded.cols,CV_8UC1,cvScalar(0.));
            
      GEO2D_Contour_t contour;
      contour.reserve(polar_contour.size());

      //std::stringstream ss5;
      //ss3 << "Inserting into contour : ";
      ::geo2d::Vector<int> this_pt,last_pt;
      for (size_t pt_idx=0; pt_idx<polar_contour.size(); ++pt_idx) {
      	auto const& polar_pt = polar_contour[pt_idx];
	
	//polar_ctor_mat.at<unsigned char>(polar_pt.y,polar_pt.x) = (unsigned char) 255;
	
      	//up case to floating point
      	float r = polar_pt.x;
      	float t = polar_pt.y;
	
      	r = (r / cols) * max_radius;
      	t = ((t / rows) * 360.0 + angle) * M_PI / 180.0;
	
      	this_pt.x = (size_t)((float) r * std::cos(t) + ref_vtx_copy.x + 0.5);
      	this_pt.y = (size_t)((float) r * std::sin(t) + ref_vtx_copy.y + 0.5);
	
      	//res_contour._contour[pt_idx].x = (int)(pt.x + 0.5) - padding;
      	//res_contour._contour[pt_idx].y = (int)(pt.y + 0.5) - padding;

	this_pt.x -= padding;
	this_pt.y -= padding;
	if(pt_idx == 0 || this_pt.x != last_pt.x || this_pt.y != last_pt.y)
	  contour.push_back(this_pt);
	last_pt = this_pt;
	
	//ss5<<"[" << res_contour._contour[pt_idx].x << "," << res_contour._contour[pt_idx].y << "],";
      }
      result_v.emplace_back(std::move(contour));
    }

    return result_v;
  }

}

#endif
