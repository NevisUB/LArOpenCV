#ifndef __REFINE2DVERTEX_CXX__
#define __REFINE2DVERTEX_CXX__

#include "Refine2DVertex.h"
#include "CircleVertex.h"
#include "DefectCluster.h"
#include "PCACandidates.h"
#include "Core/HalfLine.h"
#include "Core/Line.h"
#include "Core/BoundingBox.h"
#include "ImageClusterFMWKInterface.h"
#include <array>
#include <map>

namespace larocv{

  /// Global larocv::Refine2DVertexFactory to register ClusterAlgoFactory
  static Refine2DVertexFactory __global_Refine2DVertexFactory__;

  void Refine2DVertex::_Configure_(const ::fcllite::PSet &pset)
  {
    //auto const circle_vtx_algo_name = pset.get<std::string>("CircleVertexAlgo");
    //_circle_vertex_algo_id = this->ID(circle_vtx_algo_name);
    _defect_algo_id = _pca_algo_id = kINVALID_ID;

    auto const defect_algo_name = pset.get<std::string>("DefectClusterAlgo","");
    if(!defect_algo_name.empty()) _defect_algo_id = this->ID(defect_algo_name);

    auto const pca_algo_name = pset.get<std::string>("PCACandidatesAlgo","");
    if(!pca_algo_name.empty()) _pca_algo_id = this->ID(pca_algo_name);
    
    if(_defect_algo_id == kINVALID_ID && _pca_algo_id == kINVALID_ID) {
      LAROCV_CRITICAL() << "Must provide either PCACandidates or DefectCluster algorithm ID!" << std::endl;
      throw larbys();
    }

    _radius = 20;
    _pi_threshold = 10;
    _pca_box_size = 3;
    _global_bound_size = 25;

    _tick_offset_v.resize(3);
    _tick_offset_v[0] = 0.;
    _tick_offset_v[1] = 4.54;
    _tick_offset_v[2] = 7.78;
    //_tick_offset_v[1] = 0.;
    //_tick_offset_v[2] = 0.;
    _use_polar_spread = false;
    _wire_comp_factor_v.resize(3);
    _time_comp_factor_v.resize(3);
    _xplane_tick_resolution = 2.;
    _xplane_guess_max_dist  = 3.;
  }

  float Refine2DVertex::AngularSpread(const ::cv::Mat& polarimg,
				      float radius_frac_min,
				      float radius_frac_max,
				      float angle_mean,
				      float angle_width)
  {
    if(angle_mean < 0) angle_mean += 360.;
    float angle_min  = angle_mean - angle_width/2.;
    float angle_max  = angle_mean + angle_width/2.;
    float radius_min = radius_frac_min * (float)(polarimg.cols);
    float radius_max = radius_frac_max * (float)(polarimg.cols);
    float yfactor = (float)(polarimg.rows) / 360.;
    /*
    std::cout<<"image dim: rows=" << polarimg.rows << " cols=" << polarimg.cols << std::endl;
    std::cout<<"radius frac min=" << radius_frac_min << " max=" << radius_frac_max << std::endl;
    std::cout<<"angle min=" << angle_min << " max=" << angle_max << std::endl;
    */    
    if(angle_max > 360. && angle_min < 0.) throw larbys("Angle range too big");

    float res_mean=0;
    float res_spread=0;
    if(angle_max > 360.){
      auto box1 = ::cv::Rect(radius_min, 0, (radius_max - radius_min), yfactor * (angle_max - 360.));
      auto box2 = ::cv::Rect(radius_min, yfactor * angle_min, (radius_max - radius_min), yfactor * (360. - angle_min));
      //std::cout<<"Box1: "<<box1<<std::endl;
      //std::cout<<"Box2: "<<box2<<std::endl;

      auto small_img1 = ::cv::Mat(polarimg,box1);
      ::cv::Mat thresh_small_img1;
      ::cv::threshold(small_img1,thresh_small_img1,_pi_threshold,1,CV_THRESH_BINARY);
      geo2d::VectorArray<int> points1;
      findNonZero(thresh_small_img1, points1);

      auto small_img2 = ::cv::Mat(polarimg,box2);
      ::cv::Mat thresh_small_img2;
      ::cv::threshold(small_img2,thresh_small_img2,_pi_threshold,1,CV_THRESH_BINARY);
      geo2d::VectorArray<int> points2;
      findNonZero(thresh_small_img2, points2);

      for (unsigned i = 0; i < points1.size(); ++i)
	res_mean += (float)(points1[i].y);
      for (unsigned i = 0; i < points2.size(); ++i)
	res_mean += (float)(points2[i].y - (float)(polarimg.rows));
      res_mean /= (float)(points1.size() + points2.size());
      
      for (unsigned i = 0; i < points1.size(); ++i)
	res_spread += ((float)(points1[i].y) - res_mean) * ((float)(points1[i].y) - res_mean);
      for (unsigned i = 0; i < points2.size(); ++i)
	res_spread += ((float)(points2[i].y - (float)(polarimg.rows)) - res_mean) * ((float)(points2[i].y - (float)(polarimg.rows)) - res_mean);

      res_spread /= (float)(points1.size()+points2.size());
      
    }else if(angle_min < 0.){
      auto box1 = ::cv::Rect(radius_min, 0, (radius_max - radius_min), yfactor * angle_max);
      auto box2 = ::cv::Rect(radius_min, polarimg.rows - yfactor * (-1. * angle_min) -1., (radius_max - radius_min), yfactor * (-1. * angle_min));
      //std::cout<<"Box1: "<<box1<<std::endl;
      //std::cout<<"Box2: "<<box2<<std::endl;

      auto small_img1 = ::cv::Mat(polarimg,box1);
      ::cv::Mat thresh_small_img1;
      ::cv::threshold(small_img1,thresh_small_img1,_pi_threshold,1,CV_THRESH_BINARY);
      geo2d::VectorArray<int> points1;
      findNonZero(thresh_small_img1, points1);

      auto small_img2 = ::cv::Mat(polarimg,box2);
      ::cv::Mat thresh_small_img2;
      ::cv::threshold(small_img2,thresh_small_img2,_pi_threshold,1,CV_THRESH_BINARY);
      geo2d::VectorArray<int> points2;
      findNonZero(thresh_small_img2, points2);

      for (unsigned i = 0; i < points1.size(); ++i)
	res_mean += (float)(points1[i].y);
      for (unsigned i = 0; i < points2.size(); ++i)
	res_mean += (float)(points2[i].y - (float)(polarimg.rows));
      res_mean /= (float)(points1.size() + points2.size());
      
      for (unsigned i = 0; i < points1.size(); ++i)
	res_spread += ((float)(points1[i].y) - res_mean) * ((float)(points1[i].y) - res_mean);
      for (unsigned i = 0; i < points2.size(); ++i)
	res_spread += ((float)(points2[i].y - (float)(polarimg.rows)) - res_mean) * ((float)(points2[i].y - (float)(polarimg.rows)) - res_mean);

      res_spread /= (float)(points1.size()+points2.size());

    }else{
      auto small_img = ::cv::Mat(polarimg,::cv::Rect(radius_frac_min * (float)(polarimg.cols),
						     (angle_mean - angle_width/2.) * yfactor,
						     (radius_frac_max - radius_frac_min) * (float)(polarimg.cols),
						     angle_width * yfactor)
				 );
      ::cv::Mat thresh_small_img;
      ::cv::threshold(small_img,thresh_small_img,_pi_threshold,1,CV_THRESH_BINARY);
      geo2d::VectorArray<int> points;
      findNonZero(thresh_small_img, points);
      for (unsigned i = 0; i < points.size(); ++i)
	res_mean += (float)(points[i].y);
      res_mean /= (float)(points.size());
      for (unsigned i = 0; i < points.size(); ++i)
	res_spread += ((float)(points[i].y) - res_mean) * ((float)(points[i].y) - res_mean);
      res_spread /= (float)(points.size());
    }

    return res_spread;
  }

  geo2d::Line<float> Refine2DVertex::SquarePCA(const ::cv::Mat& img,
					       geo2d::Vector<float> pt,
					       float width, float height)
  {
					       
    auto small_img = ::cv::Mat(img,::cv::Rect(pt.x-width, pt.y-height, 2*width+1, 2*height+1));
    ::cv::Mat thresh_small_img;
    //::cv::threshold(small_img,thresh_small_img,_pi_threshold,1,CV_THRESH_BINARY);
    ::cv::threshold(small_img,thresh_small_img,1,1000,3);
    geo2d::VectorArray<int> points;
    findNonZero(thresh_small_img, points);

    if(points.size() < 2) {
      LAROCV_DEBUG() << "SquarePCA approx cannot be made (# points " << points.size() << " < 2)" << std::endl;
      throw larbys("SquarePCA found no point...");
    }
    
    cv::Mat mat_points(points.size(), 2, CV_32FC1);
    for (unsigned i = 0; i < mat_points.rows; ++i) {
      mat_points.at<float>(i, 0) = points[i].x;
      mat_points.at<float>(i, 1) = points[i].y;
    }

    ::cv::PCA pcaobj(mat_points,::cv::Mat(),::cv::PCA::DATA_AS_ROW,0);
    
    pt.x += pcaobj.mean.at<float>(0,0) - width;
    pt.y += pcaobj.mean.at<float>(0,1) - height;
    
    auto dir = geo2d::Vector<float>(pcaobj.eigenvectors.at<float>(0,0),
				    pcaobj.eigenvectors.at<float>(0,1));

    //LAROCV_DEBUG() << "Mean @ (" << pt.x << "," << pt.y << ") ... dir (" << dir.x << "," << dir.y << std::endl;

    return geo2d::Line<float>(pt,dir);
  }

  geo2d::VectorArray<float> Refine2DVertex::QPointOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle)
  {
    geo2d::VectorArray<float> res;
    // Find crossing point
    ::cv::Mat polarimg;
    ::cv::linearPolar(img, polarimg, circle.center, _radius*2, ::cv::WARP_FILL_OUTLIERS);

    size_t col = (size_t)(polarimg.cols / 2);
			  
    std::vector<std::pair<int,int> > range_v;
    std::pair<int,int> range(-1,-1);
    for(size_t row=0; row<polarimg.rows; ++row) {

      float q = (float)(polarimg.at<unsigned char>(row, col));
      if(q < _pi_threshold) {
	if(range.first >= 0) {
	  range_v.push_back(range);
	  range.first = range.second = -1;
	}
	continue;
      }
      //std::cout << row << " / " << polarimg.rows << " ... " << q << std::endl;
      if(range.first < 0) range.first = range.second = row;

      else range.second = row;

    }
    // Check if end should be combined w/ start
    if(range_v.size() >= 2) {
      if(range_v[0].first == 0 && (range_v.back().second+1) == polarimg.rows) {
	range_v[0].first = range_v.back().first - (int)(polarimg.rows);
	range_v.pop_back();
      }
    }
    // Compute xs points
    for(auto const& r : range_v) {

      //std::cout << "XS @ " << r.first << " => " << r.second << " ... " << polarimg.rows << std::endl;
      float angle = ((float)(r.first + r.second))/2.;
      if(angle < 0) angle += (float)(polarimg.rows);
      angle = angle * M_PI * 2. / ((float)(polarimg.rows));

      geo2d::Vector<float> pt;
      pt.x = circle.center.x + _radius * cos(angle);
      pt.y = circle.center.y + _radius * sin(angle);

      res.push_back(pt);      
    }
    return res;
  }

  geo2d::Vector<float> Refine2DVertex::MeanPixel(const cv::Mat& img, const geo2d::Vector<float>& center)
  {
    // Make a better guess
    auto small_img = ::cv::Mat(img,::cv::Rect(center.x - 2, center.y - 2, 5, 5));
    ::cv::Mat thresh_small_img;
    ::cv::threshold(small_img,thresh_small_img,_pi_threshold,1,CV_THRESH_BINARY);
    //::cv::threshold(small_img,thresh_small_img,_pi_threshold,1000,3);
    
    geo2d::VectorArray<int> points;
    findNonZero(thresh_small_img, points);
    
    geo2d::Vector<float> trial_pt(-1,-1);
    if(points.empty()) return trial_pt;
    
    for (unsigned i = 0; i < points.size(); ++i){
      trial_pt.x += points[i].x;
      trial_pt.y += points[i].y;
    }
    
    trial_pt.x /= (float)(points.size());
    trial_pt.y /= (float)(points.size());
    
    trial_pt.x = center.x + trial_pt.x - 2;
    trial_pt.y = center.y + trial_pt.y - 2;

    return trial_pt;
  }

  double Refine2DVertex::PointInspection(const cv::Mat& img, const geo2d::Vector<float>& pt)
  {

    // Get xs points for this step. if < 2 continue
    auto const xs_pts = QPointOnCircle(img,geo2d::Circle<float>(pt,_radius));
    
    if(xs_pts.size()<2) { return -1; }
    
    if(xs_pts.size()==2) {
      auto center_line0 = geo2d::Line<float>(xs_pts[0], xs_pts[0] - pt);
      auto center_line1 = geo2d::Line<float>(xs_pts[1], xs_pts[1] - pt);
      auto dtheta = fabs(geo2d::angle(center_line0) - geo2d::angle(center_line1));
      if(dtheta < 10) { return -1; }
    }
    
    double dtheta_sum = 0;
    if(_use_polar_spread) {
      // Estimate a polar y-spread as am easure of "dtheta"
      ::cv::Mat local_polarimg;
      ::cv::linearPolar(img, local_polarimg, pt, _radius+10., ::cv::WARP_FILL_OUTLIERS);
      for(auto const& xs_pt : xs_pts) {
	float angle = geo2d::angle(pt,xs_pt);
	float radius_frac_min = (_radius - 2.) / (_radius+10.);
	float radius_frac_max = (_radius + 3.) / (_radius+10.);
	dtheta_sum += AngularSpread(local_polarimg,radius_frac_min,radius_frac_max,angle,10.);
      }
    }else{
      // Estimate a local PCA and center-to-xs line's angle diff
      for(size_t xs_idx=0; xs_idx < xs_pts.size(); ++xs_idx) {
	auto const& xs_pt = xs_pts[xs_idx];
	// Line that connects center to xs
	try{
	  auto local_pca   = SquarePCA(img,xs_pt,_pca_box_size,_pca_box_size);
	  auto center_line = geo2d::Line<float>(xs_pt, xs_pt - pt);
	  dtheta_sum += fabs(geo2d::angle(center_line) - geo2d::angle(local_pca));
	  // Alternative (and probably better/faster): compute y spread in polar coordinate
	  LAROCV_DEBUG() << "Line ID " << xs_idx << " found xs " << xs_pt << " dtheta " << fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)) << std::endl;
	}catch(const larbys& err){
	  return -1;
	}
      }
    }
    
    dtheta_sum /= (float)(xs_pts.size());
    return dtheta_sum;
  }

  double Refine2DVertex::TwoPointInspection(const cv::Mat& img, const geo2d::Vector<float>& pt)
  {
    // Get xs points for this step. if < 2 continue
    auto const inner_xs_pts = QPointOnCircle(img,geo2d::Circle<float>(pt,_radius));
    if(inner_xs_pts.size()<2) { return -1; }    
    if(inner_xs_pts.size()==2) {
      auto center_line0 = geo2d::Line<float>(inner_xs_pts[0], inner_xs_pts[0] - pt);
      auto center_line1 = geo2d::Line<float>(inner_xs_pts[1], inner_xs_pts[1] - pt);
      auto dtheta = fabs(geo2d::angle(center_line0) - geo2d::angle(center_line1));
      if(dtheta < 10) { return -1; }
    }

    auto const outer_xs_pts = QPointOnCircle(img,geo2d::Circle<float>(pt,_radius*2));
    
    double inner_dtheta_sum = 0;
    double outer_dtheta_sum = 0;
    if(_use_polar_spread) {
      // Estimate a polar y-spread as am easure of "dtheta"
      ::cv::Mat local_polarimg;
      ::cv::linearPolar(img, local_polarimg, pt, _radius*2.5, ::cv::WARP_FILL_OUTLIERS);
      for(auto const& xs_pt : inner_xs_pts) {
	float angle = geo2d::angle(pt,xs_pt);
	float radius_frac_min = (_radius - 2.) / (_radius*2.5);
	float radius_frac_max = (_radius + 3.) / (_radius*2.5);
	inner_dtheta_sum += AngularSpread(local_polarimg,radius_frac_min,radius_frac_max,angle,10.);
      }
      for(auto const& xs_pt : outer_xs_pts) {
	float angle = geo2d::angle(pt,xs_pt);
	float radius_frac_min = (_radius*2 - 2.) / (_radius*2.5);
	float radius_frac_max = (_radius*2 + 3.) / (_radius*2.5);
	outer_dtheta_sum += AngularSpread(local_polarimg,radius_frac_min,radius_frac_max,angle,10.);
      }
    }else{
      // Estimate a local PCA and center-to-xs line's angle diff
      for(size_t xs_idx=0; xs_idx < inner_xs_pts.size(); ++xs_idx) {
	auto const& xs_pt = inner_xs_pts[xs_idx];
	// Line that connects center to xs
	try{
	  auto local_pca   = SquarePCA(img,xs_pt,_pca_box_size,_pca_box_size);
	  auto center_line = geo2d::Line<float>(xs_pt, xs_pt - pt);
	  inner_dtheta_sum += fabs(geo2d::angle(center_line) - geo2d::angle(local_pca));
	  // Alternative (and probably better/faster): compute y spread in polar coordinate
	  LAROCV_DEBUG() << "Line (inner) ID " << xs_idx << " found xs " << xs_pt << " dtheta " << fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)) << std::endl;
	}catch(const larbys& err){
	  return -1;
	}
      }
      // Estimate a local PCA and center-to-xs line's angle diff
      for(size_t xs_idx=0; xs_idx < outer_xs_pts.size(); ++xs_idx) {
	auto const& xs_pt = outer_xs_pts[xs_idx];
	// Line that connects center to xs
	try{
	  auto local_pca   = SquarePCA(img,xs_pt,_pca_box_size,_pca_box_size);
	  auto center_line = geo2d::Line<float>(xs_pt, xs_pt - pt);
	  outer_dtheta_sum += fabs(geo2d::angle(center_line) - geo2d::angle(local_pca));
	  // Alternative (and probably better/faster): compute y spread in polar coordinate
	  LAROCV_DEBUG() << "Line (outer) ID " << xs_idx << " found xs " << xs_pt << " dtheta " << fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)) << std::endl;
	}catch(const larbys& err){
	  return -1;
	}
      }
    }
    
    //dtheta_sum /= (float)(xs_pts.size());
    return (inner_dtheta_sum + outer_dtheta_sum) / ((float)(inner_xs_pts.size() + outer_xs_pts.size()));
  }

  bool Refine2DVertex::PlaneScan(const ::cv::Mat& img, const size_t plane,
				 const geo2d::Circle<float> init_circle,
				 const geo2d::Vector<float> pt_err)
  {
    bool res = false;
    
    LAROCV_DEBUG() << "PlaneScan running on plane " << plane
		   << " @ point " << init_circle.center << std::endl;

    auto& scan_marker = _scan_marker_v[plane];
    
    auto& plane_data  = AlgoData<larocv::Refine2DVertexData>()._plane_data.at(plane);
    auto& init_xs_vv  = plane_data._init_xs_vv;
    auto& init_pca_vv = plane_data._init_pca_vv;
    auto& init_vtx_v  = plane_data._init_vtx_v;
    auto& scan_rect_v = plane_data._scan_rect_v;
    auto& circle_trav_v = plane_data._circle_trav_v;
    auto& dtheta_trav_v = plane_data._dtheta_trav_v;

    // Initialize variables for this scan
    init_vtx_v.resize(init_vtx_v.size()+1);
    init_xs_vv.resize(init_xs_vv.size()+1);
    init_pca_vv.resize(init_xs_vv.size()+1);

    auto& init_vtx   = init_vtx_v.back();
    auto& init_xs_v  = init_xs_vv.back();
    auto& init_pca_v = init_pca_vv.back();

    init_vtx = init_circle.center;

    //data._circle_trav_vv[plane].clear();
    //data._dtheta_trav_vv[plane].clear();
    //init_pca_v.clear();
    //init_xs_v.clear();

    // Create another circle with 1/2 radius
    auto half_circle = init_circle;
    half_circle.radius *= 0.5;

    // Find xs point on the initial circle
    geo2d::VectorArray<float> temp1_xs_v, temp2_xs_v;
    std::vector<geo2d::Line<float>   > temp1_pca_v, temp2_pca_v;

    // Find valid PCAs per xs point 
    for(auto const& pt : QPointOnCircle(img,init_circle)) {
      try{
	auto const line = SquarePCA(img,pt,_pca_box_size,_pca_box_size);
	temp1_xs_v.push_back(pt);
	temp1_pca_v.push_back(line);
      }catch(const larbys& err){
	continue;
      }
    }
    
    // Find valid PCAs per xs point 
    for(auto const& pt : QPointOnCircle(img,half_circle)) {
      try{
	auto const line = SquarePCA(img,pt,_pca_box_size,_pca_box_size);
	temp2_xs_v.push_back(pt);
	temp2_pca_v.push_back(line);
      }catch(const larbys& err){
	continue;
      }
    }

    // If # xs points are larger in one of two, pick that one.
    // Else pick a larger circle (likely have a better angle estimate)
    if(temp1_xs_v.size() == temp2_xs_v.size()) {
      init_xs_v  = temp1_xs_v;
      init_pca_v = temp1_pca_v;
    }else{
      init_xs_v  = (temp1_xs_v.size()  > temp2_xs_v.size()  ? temp1_xs_v  : temp2_xs_v );
      init_pca_v = (temp1_pca_v.size() > temp2_pca_v.size() ? temp1_pca_v : temp2_pca_v);
    }

    // Define the (x,y) region to traverse along initial pca
    geo2d::Vector<float> top_left(init_vtx.x - _global_bound_size,
				  init_vtx.y + _global_bound_size);
    geo2d::Vector<float> bottom_right(init_vtx.x + _global_bound_size,
				      init_vtx.y - _global_bound_size);
    
    geo2d::Rect bbox(top_left, bottom_right);
    scan_rect_v.push_back(bbox);
    
    LAROCV_DEBUG() << "Plane " << plane << " scanning bbox:" << bbox << std::endl;
    // Now we loop over per-xs-on-init-circle, and find a candidate vertex per xs point
    //std::multimap< double, geo2d::Vector<float> > candidate_vtx_m;
    for(size_t xs_idx=0; xs_idx < init_xs_v.size(); ++xs_idx){

      auto const& xs  = init_xs_v [xs_idx];
      auto const& pca = init_pca_v[xs_idx];

      // Get a unit vector to traverse, starting from X-min edge point
      geo2d::Vector<float> start,end;
      if(pca.dir.y == 1) {
	start.x = xs.x;
	start.y = top_left.y;
	end.x   = xs.x;
	end.y   = bottom_right.y;
      }else{
	start.x = top_left.x;     start.y = pca.y(top_left.x);
	end.x   = bottom_right.x; end.y   = pca.y(bottom_right.x);
      }
      auto const dir = ::geo2d::dir(start,end);
      
      LAROCV_DEBUG() << "XS point " << xs_idx << " ... "
		     <<"start: " << start << " ... "
		     << "end: " << end << " ... "
		     << "dir: " << dir << std::endl;
      
      // Make sure start/end is on the box sides
      auto& upper_pt = (start.y > end.y ? start : end);
      auto& lower_pt = (start.y < end.y ? start : end);
      if(upper_pt.y > top_left.y) {
	upper_pt.y = top_left.y;
	upper_pt.x = pca.x(upper_pt.y);
      }
      if(lower_pt.y < bottom_right.y) {
	lower_pt.y = bottom_right.y;
	lower_pt.x = pca.x(lower_pt.y);
      }

      size_t row, col;
      float q;
      auto step_pt = start + dir / 2.;
      //double min_dtheta_sum = 1e4;
      //geo2d::Vector<float> candidate_vtx = start;
      
      while(bbox.contains(step_pt)) {

	// Check if this point has any charge. if not continue
	col = (size_t)(step_pt.x);
	row = (size_t)(step_pt.y);

	size_t marker_index = row + col * img.rows;
	if(scan_marker[marker_index]) {
	  step_pt += dir;
	  continue;
	}
	
	scan_marker[marker_index] = true;
	
	q = (float)(img.at<unsigned char>(row,col));

	if(q < _pi_threshold) {
	  step_pt += dir;
	  continue;
	}

	auto trial_pt = this->MeanPixel(img,step_pt);

	auto dtheta_sum = this->TwoPointInspection(img,trial_pt);

	if(dtheta_sum < 0) {
	  step_pt+= dir;
	  continue;
	}

	LAROCV_DEBUG() << "    ... dtheta_sum = " << dtheta_sum << std::endl;
	circle_trav_v.emplace_back(std::move(geo2d::Circle<float>(trial_pt,_radius)));
	dtheta_trav_v.push_back(dtheta_sum);
	res = true;

	/*
	if(dtheta_sum < min_dtheta_sum) {
	  min_dtheta_sum = dtheta_sum;
	  candidate_vtx = trial_pt;
	}
	*/	
	// Increment the step and continue or break
	step_pt += dir;	
      }
    }
    return res;
  }

  void Refine2DVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
				 const ::cv::Mat& img,
				 larocv::ImageMeta& meta,
				 larocv::ROI& roi)
  {
    auto& data = AlgoData<larocv::Refine2DVertexData>();

    if(_scan_marker_v.size() <= meta.plane())
      _scan_marker_v.resize(meta.plane()+1);

    auto& scan_marker = _scan_marker_v[meta.plane()];
    scan_marker.resize((size_t)(img.rows) * (size_t)(img.cols),false);

    for(size_t idx=0; idx<scan_marker.size(); ++idx)
      scan_marker[idx] = false;

    _time_comp_factor_v[meta.plane()] = meta.pixel_height();
    _wire_comp_factor_v[meta.plane()] = meta.pixel_width();
    LAROCV_DEBUG() << "Plane " << meta.plane()
		   << " wire comp factor: " << _wire_comp_factor_v[meta.plane()]
		   << " time comp factor: " << _time_comp_factor_v[meta.plane()]
		   << std::endl;

    geo2d::VectorArray<float> pt_v;
    geo2d::VectorArray<float> pt_err_v;
    geo2d::Vector<float> pt_err(2.,2.);

    bool found=false;
    geo2d::Circle<float> circle;
    circle.radius = _radius;

    if(_defect_algo_id != kINVALID_ID) {
      auto const& defect_pts = AlgoData<larocv::DefectClusterData>(_defect_algo_id);
      for(auto const& pts : defect_pts._atomic_defect_pts_v_v_v[meta.plane()]) {
	for(auto const& pt : pts) {
	  LAROCV_INFO() << "Scanning Defect point: " << pt << std::endl;
	  circle.center = pt;
	  found = PlaneScan(img,meta.plane(),circle,pt_err) || found;
	}
      }
    }

    if(_pca_algo_id != kINVALID_ID) {
      auto const& pca_pts    = AlgoData<larocv::PCACandidatesData>(_pca_algo_id);    
      for(auto const& pt : pca_pts._ipoints_v_v[meta.plane()]) {
	LAROCV_INFO() << "Scanning PCACandidate point: " << pt << std::endl;
	circle.center = pt;
	found = PlaneScan(img,meta.plane(),circle,pt_err) || found;
      }
    }
    
    if(!found) {
      data._plane_data.at(meta.plane())._valid_plane = false;
      return;
    }

    data._plane_data.at(meta.plane())._valid_plane = true;

  }

  void Refine2DVertex::XPlaneTimeScan(const std::vector<const cv::Mat>& img_v)
  {
    // Combine 3 plane information and make the best guesses as to which time tick may contain vertex(es)
    // To do this, we loop over time tick over all available planes first
    auto& data = AlgoData<larocv::Refine2DVertexData>();
    auto& plane_data_v = data._plane_data;
    // Initialize results
    for(size_t plane=0; plane<img_v.size(); ++plane) {
      auto& plane_data = data._plane_data.at(plane);
      auto& time_binned_score_v = plane_data._time_binned_score_v;
      time_binned_score_v.clear();
    }

    // Compute tick offset per plane
    auto tick_offset_v = _tick_offset_v; // should be a copy
    for(size_t plane=0; plane<img_v.size(); ++plane) {
      tick_offset_v[plane] /= _time_comp_factor_v[plane];
      LAROCV_DEBUG() << "Plane " << plane
		     << " offset = " << tick_offset_v[plane]
		     << " @ compression " << _time_comp_factor_v[plane]
		     << std::endl;
    }

    // First obtain inclusive overlapping time tick regions
    auto& xplane_tick_min = data._xplane_tick_min;
    auto& xplane_tick_max = data._xplane_tick_max;
    
    xplane_tick_min = -1;
    xplane_tick_max = -1;
    for(size_t plane=0; plane<img_v.size(); ++plane){
      auto const& plane_data = plane_data_v.at(plane);
      if(!plane_data._valid_plane) continue;
      for(auto const& bbox : plane_data._scan_rect_v) {

	LAROCV_DEBUG() << "Plane " << plane << " scanned within a box " << bbox << std::endl;
	float ref_x = bbox.x;
	ref_x -= tick_offset_v[plane];
	/*
	// This is exclusive boundary
	if(xplane_tick_min < 0) xplane_tick_min = ref_x;
	else if(xplane_tick_min < ref_x) xplane_tick_min = ref_x;
	if(xplane_tick_max < 0) xplane_tick_max = ref_x + bbox.width;
	else if(xplane_tick_max > (ref_x + bbox.width)) xplane_tick_max = ref_x + bbox.width;
	*/
	if(xplane_tick_min < 0) xplane_tick_min = ref_x;
	else if(xplane_tick_min > ref_x) xplane_tick_min = ref_x;
	if(xplane_tick_max < 0) xplane_tick_max = ref_x + bbox.width;
	else if(xplane_tick_max < (ref_x + bbox.width)) xplane_tick_max = ref_x + bbox.width;
      }
    }
    
    // Next define score map within a pre-defined tick resolution
    const size_t num_ticks = (xplane_tick_max - xplane_tick_min)/_xplane_tick_resolution + 1;
    LAROCV_DEBUG() << "NumTicks to scan in time: " << num_ticks << " (" << xplane_tick_min << " => " << xplane_tick_max << ")" << std::endl;

    for(size_t plane=0; plane<img_v.size(); ++plane) {
      auto& plane_data = plane_data_v.at(plane);
      auto& time_binned_score_v = plane_data._time_binned_score_v;
      time_binned_score_v.resize(num_ticks+1);
      for(size_t tick=0; tick<=num_ticks; ++tick)
	time_binned_score_v[tick] = -1;
    }
    
    for(size_t plane=0; plane<img_v.size(); ++plane) {
      auto& plane_data = plane_data_v.at(plane);
      std::cout<<"Plane " << plane << " ..."<<std::endl;
      if(!plane_data._valid_plane) continue;
      auto const& dtheta_trav_v  = plane_data._dtheta_trav_v;
      auto const& circle_trav_v  = plane_data._circle_trav_v;
      auto const& tick_offset    = tick_offset_v.at(plane);
      auto&       time_binned_score_v = plane_data._time_binned_score_v;
      
      for(size_t step=0; step<circle_trav_v.size(); ++step) {
	auto const& dtheta = dtheta_trav_v[step];
	auto const& circle = circle_trav_v[step];
	float tick = circle.center.x - tick_offset;
	size_t tick_idx = (size_t)((tick - xplane_tick_min)/_xplane_tick_resolution + 0.5);
	if(tick < xplane_tick_min || tick > xplane_tick_max) {
	  LAROCV_DEBUG() << "tick @ " << tick << " (array idx=" << tick_idx << ") .... dtheta @ " << dtheta <<  " ... skipping (out-o-range) " << std::endl;
	  continue;
	}

	auto& time_binned_score = time_binned_score_v[tick_idx];
	if(time_binned_score > 0 && time_binned_score < dtheta) {
	  LAROCV_DEBUG() << "tick @ " << tick << " (array idx=" << tick_idx << ") .... dtheta @ " << dtheta <<  " ... skipping (dtheta too big) " << std::endl;
	  continue;
	}
	LAROCV_DEBUG() << "tick @ " << tick << " (array idx=" << tick_idx << ") .... dtheta @ " << dtheta <<  " ... registering " << std::endl;
	time_binned_score = dtheta;
      }
    }

    // Now loop over each "tick" and compute the best matching vertex point
    size_t num_valid_planes = 0;
    float  dtheta_sum = 0.;
    float  min_dtheta_sum = 1e4;
    float  min_dtheta_sum_1plane=1e4;
    size_t min_dtheta_sum_tick = kINVALID_SIZE;
    size_t min_dtheta_sum_1plane_tick = kINVALID_SIZE;
    auto& xplane_time_binned_score0_v = data._xplane_time_binned_score0_v;
    auto& xplane_time_binned_score1_v = data._xplane_time_binned_score1_v;
    xplane_time_binned_score0_v.clear();
    xplane_time_binned_score1_v.clear();
    xplane_time_binned_score0_v.resize(num_ticks+1,-1);
    xplane_time_binned_score1_v.resize(num_ticks+1,-1);
    for(size_t tick=0; tick < num_ticks; ++tick) {
      num_valid_planes = 0;
      dtheta_sum = 0.;
      LAROCV_DEBUG() << "At tick " << tick << "/" << num_ticks << std::endl;
      for(size_t plane=0; plane < img_v.size(); ++plane) {
	auto const& plane_data = data._plane_data[plane];
	if(!plane_data._valid_plane) continue;
	auto const& time_binned_score_v = plane_data._time_binned_score_v;
	LAROCV_DEBUG() << "    Plane " << plane << " dtheta = " << time_binned_score_v[tick] << std::endl;
	if(time_binned_score_v[tick] < 0) continue;
	dtheta_sum += time_binned_score_v[tick];
	num_valid_planes +=1;
      }
      LAROCV_DEBUG() << "    # valid plane = " << num_valid_planes << " dtheta = " << dtheta_sum << std::endl;

      // compute average dtheta_sum
      dtheta_sum /= (float)(num_valid_planes);

      if(num_valid_planes < 1) continue;
      xplane_time_binned_score0_v[tick] = dtheta_sum;
      if(dtheta_sum < min_dtheta_sum_1plane) {
	min_dtheta_sum_1plane = dtheta_sum;
	min_dtheta_sum_1plane_tick = tick;
	LAROCV_DEBUG() << "    ... new minimum (1plane)!" << std::endl;
      }

      if(num_valid_planes < 2) continue;
      xplane_time_binned_score1_v[tick] = dtheta_sum;
      if(dtheta_sum > min_dtheta_sum) continue;
      LAROCV_DEBUG() << "    ... new minimum (>1plane)!" << std::endl;
      min_dtheta_sum = dtheta_sum;
      min_dtheta_sum_tick = tick;
    }

    return;
    /*
    size_t target_tick = kINVALID_SIZE;
    if(min_dtheta_sum_tick != kINVALID_SIZE) target_tick = min_dtheta_sum_tick;
    else target_tick = min_dtheta_sum_1plane_tick;

    if(target_tick == kINVALID_SIZE) return;
    
    // Now record best vtx point per plane
    for(size_t plane=0; plane < xplane_vtx_vv.size(); ++plane) {
      data._cand_valid_v[plane] = false;
      if(!data._valid_plane_v[plane]) continue;
      if(xplane_score_vv[plane][target_tick]<0) continue;
      
      data._cand_vtx_v[plane] = xplane_vtx_vv[plane][target_tick];
      data._cand_score_v[plane] = xplane_score_vv[plane][target_tick];
      LAROCV_DEBUG() << "Vtx @ plane " << plane << " ... " << data._cand_vtx_v[plane]
		     << " @ dtheta " << data._cand_score_v[plane] << std::endl;

      // Obtain crossing point w/ a circle around the vertex. These are particle cluster candidates.
      geo2d::Circle<float> circle;
      circle.center = data._cand_vtx_v[plane];
      circle.radius = _radius / 2.;
      data._cand_xs_vv[plane] = QPointOnCircle(img_v[plane],circle);
      data._cand_valid_v[plane] = true;
    }
    */
  }

  bool Refine2DVertex::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    XPlaneTimeScan(img_v);
    return true;
  }

}
#endif
