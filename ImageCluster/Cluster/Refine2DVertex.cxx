#ifndef __REFINE2DVERTEX_CXX__
#define __REFINE2DVERTEX_CXX__

#include "Refine2DVertex.h"
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

    _trigger_tick = 2400;

    _radius = 10;
    _pi_threshold = 10;
    _pca_box_size = 3;
    _global_bound_size = 20;

    _tick_offset_v.resize(3);
    _tick_offset_v[0] = 0.;
    _tick_offset_v[1] = 4.54;
    _tick_offset_v[2] = 7.78;
    //_tick_offset_v[1] = 0.;
    //_tick_offset_v[2] = 0.;
    _use_polar_spread = false;
    _wire_comp_factor_v.resize(3);
    _time_comp_factor_v.resize(3);
    _xplane_tick_resolution = 1.;
    _xplane_wire_resolution = 2.;
    _xplane_guess_max_dist  = 3.;
    _time_exclusion_radius = 20;
    _wire_exclusion_radius = 20;

    _require_3planes = false;
    _vtx_3d_resolution = 10.;

    _seed_plane_v.resize(3);
    _seed_plane_v[0] = 2;
    _seed_plane_v[1] = 0;
    _seed_plane_v[2] = 1;
  }

  std::vector<float> Refine2DVertex::RollingMean(const std::vector<float>& array,
						 size_t pre, size_t post,
						 float ignore_value)
  {
    std::vector<float> res(array.size(),ignore_value);
    float  local_sum = 0;
    size_t valid_ctr = 0;
    for(size_t idx=0; idx<array.size(); ++idx) {
      if(array[idx] == ignore_value) continue;
      if(idx < pre) continue;
      if((idx+post) >= array.size()) continue;
      local_sum = 0;
      valid_ctr = 0;
      for(size_t subidx=(idx-pre); subidx <= idx+post; ++subidx) {
	if(array[subidx] == ignore_value) continue;
	local_sum += array[subidx];
	++valid_ctr;
      }
      if(valid_ctr < 2) continue;
      res[idx] = local_sum / (float)(valid_ctr);
    }
    return res;
  }

  std::vector<float> Refine2DVertex::RollingGradient(const std::vector<float>& array,
						     size_t pre, size_t post,
						     float ignore_value)
  {
    std::vector<float> slope(array.size(),ignore_value);
    float  local_sum = 0;
    size_t valid_ctr = 0;
    float  post_mean = 0;
    float  pre_mean  = 0;
    for(size_t idx=0; idx<array.size(); ++idx) {
      if(array[idx] == ignore_value) continue;
      if(idx < pre) continue;
      if(idx+post >= array.size()) continue;
      // Compute average in pre sample
      local_sum = 0;
      valid_ctr = 0;
      for(size_t subidx=(idx-pre); subidx < idx; ++subidx){
	if(array[subidx] == ignore_value) continue;
	local_sum += array[subidx];
	++valid_ctr;
      }
      if(valid_ctr < 2) continue;
      pre_mean = local_sum / (float)(valid_ctr);
      // Compute average in post sample
      local_sum = 0;
      valid_ctr = 0;
      for(size_t subidx=idx+1; subidx <= idx+post; ++subidx){
	if(array[subidx] == ignore_value) continue;
	local_sum += array[subidx];
	++valid_ctr;
      }
      if(valid_ctr < 2) continue;
      post_mean = local_sum / (float)(valid_ctr);

      slope[idx] = (post_mean - pre_mean) / (((float)pre)/2. + ((float)post)/2. + 1.);
    }
    return slope;
  }
  
  std::vector<size_t> Refine2DVertex::ExtremePoints(const std::vector<float>& array,
						    size_t pre, size_t post, bool minimum,
						    float invalid_value)
  {
    auto const& slope = RollingGradient(array,pre,post,invalid_value);
    std::vector<size_t> res_index;
    float preval, postval;
    int prestep, poststep;
    bool found;
    for(size_t idx=0; idx<array.size(); ++idx) {
      //std::cout << "dtheta: " << array[idx] << " slope: " << slope[idx] << " @ idx " << idx << std::endl;
      if(array[idx] == invalid_value) continue;
      if(slope[idx] == invalid_value) continue;
      
      preval  = 0.;
      prestep = idx-1;
      found = false;
      while(prestep>=0 && prestep>((int)idx - pre - 1)) {
	if(slope[prestep]==invalid_value) {
	  prestep -=1;
	  continue;
	}
	preval = slope[prestep];
	found=true;
	break;
      }
      if(!found) continue;
      
      postval = 0.;
      poststep = idx+1;
      found=false;
      while(poststep<=slope.size() && poststep < ((int)idx + post +1)) {
	if(slope[poststep]==invalid_value) {
	  poststep +=1;
	  continue;
	}
	postval = slope[poststep];
	found=true;
	break;
      }
      if(!found) continue;

      if(  minimum && preval < 0 and postval > 0 ) res_index.push_back(idx);
      if( !minimum && preval > 0 and postval < 0 ) res_index.push_back(idx);
    }
    return res_index;
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

    cv::Rect rect(pt.x-width, pt.y-height, 2*width+1, 2*height+1);
    edge_rect(img,rect,2*width+1,2*height+1);
    
    auto small_img = ::cv::Mat(img,rect);
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

  void Refine2DVertex::edge_rect(const ::cv::Mat& img, cv::Rect& rect,int w, int h) {

    //make it edge aware
    if ( rect.x < 0 ) rect.x = 0;
    if ( rect.y < 0 ) rect.y = 0;
    
    if ( rect.x > img.cols - w ) rect.x = img.cols - w;
    if ( rect.y > img.rows - h ) rect.y = img.rows - h;
    
  }
  
  geo2d::Vector<float> Refine2DVertex::MeanPixel(const cv::Mat& img, const geo2d::Vector<float>& center)
  {
    // Make a better guess
    ::cv::Rect rect(center.x - 2, center.y - 2, 5, 5);
    edge_rect(img,rect,5,5);
    
    LAROCV_DEBUG() << "rows cols " << img.rows << "," << img.cols << " and rect " << rect << std::endl;
    auto small_img = ::cv::Mat(img,rect);
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

  data::CircleVertex Refine2DVertex::TwoPointInspection(const cv::Mat& img, const geo2d::Vector<float>& pt)
  {
    data::CircleVertex invalid_circle, inner_circle, outer_circle;
    invalid_circle.Clear();
    inner_circle.Clear();
    outer_circle.Clear();
    inner_circle.center = outer_circle.center = pt;
    inner_circle.radius = _radius;
    outer_circle.radius = _radius * 2.;
    // Get xs points for this step. if < 2 continue
    auto const inner_xs_pts = QPointOnCircle(img,geo2d::Circle<float>(pt,_radius));
    if(inner_xs_pts.size()<2) { return invalid_circle; }    
    if(inner_xs_pts.size()==2) {
      auto center_line0 = geo2d::Line<float>(inner_xs_pts[0], inner_xs_pts[0] - pt);
      auto center_line1 = geo2d::Line<float>(inner_xs_pts[1], inner_xs_pts[1] - pt);
      auto dtheta = fabs(geo2d::angle(center_line0) - geo2d::angle(center_line1));
      if(dtheta < 10) { return invalid_circle; }
    }

    auto const outer_xs_pts = QPointOnCircle(img,geo2d::Circle<float>(pt,_radius*2));
    double inner_dtheta_sum = 0;
    double outer_dtheta_sum = 0;
    /*
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
    */

    // Estimate a local PCA and center-to-xs line's angle diff
    for(size_t xs_idx=0; xs_idx < inner_xs_pts.size(); ++xs_idx) {
      auto const& xs_pt = inner_xs_pts[xs_idx];
      // Line that connects center to xs
      try{
	auto local_pca   = SquarePCA(img,xs_pt,_pca_box_size,_pca_box_size);
	auto center_line = geo2d::Line<float>(xs_pt, xs_pt - pt);
	// Alternative (and probably better/faster): compute y spread in polar coordinate
	LAROCV_DEBUG() << "Line (inner) ID " << xs_idx << " found xs " << xs_pt << " dtheta " << fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)) << std::endl;
	inner_circle.xs_v.push_back(data::PointPCA(xs_pt,local_pca));
	inner_circle.dtheta_v.push_back(fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)));
	inner_dtheta_sum += inner_circle.dtheta_v.back();
      }catch(const larbys& err){
	return invalid_circle;
      }
    }
    // Estimate a local PCA and center-to-xs line's angle diff
    for(size_t xs_idx=0; xs_idx < outer_xs_pts.size(); ++xs_idx) {
      auto const& xs_pt = outer_xs_pts[xs_idx];
      // Line that connects center to xs
      try{
	auto local_pca   = SquarePCA(img,xs_pt,_pca_box_size,_pca_box_size);
	auto center_line = geo2d::Line<float>(xs_pt, xs_pt - pt);
	// Alternative (and probably better/faster): compute y spread in polar coordinate
	LAROCV_DEBUG() << "Line (outer) ID " << xs_idx << " found xs " << xs_pt << " dtheta " << fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)) << std::endl;
	outer_circle.xs_v.push_back(data::PointPCA(xs_pt,local_pca));
	outer_circle.dtheta_v.push_back(fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)));
	outer_dtheta_sum += outer_circle.dtheta_v.back();
      }catch(const larbys& err){
	return invalid_circle;
      }
    }
    
    //dtheta_sum /= (float)(xs_pts.size());
    //return (inner_dtheta_sum + outer_dtheta_sum) / ((float)(inner_xs_pts.size() + outer_xs_pts.size()));

    // Pick which one to return ... 0) more xs points is better, else 1) outer is better
    if(inner_circle.xs_v.size() > outer_circle.xs_v.size()) return inner_circle;
    return outer_circle;
  }

  bool Refine2DVertex::PlaneScan(const ::cv::Mat& img, const size_t plane,
				 const geo2d::Circle<float> init_circle,
				 const geo2d::Vector<float> pt_err)
  {
    bool res = false;
    
    LAROCV_DEBUG() << "PlaneScan running on plane " << plane
		   << " @ point " << init_circle.center << std::endl;

    auto& scan_marker = _scan_marker_v[plane];
    auto& plane_data  = AlgoData<data::Refine2DVertexData>().get_plane_data_writeable(plane);
    auto& init_vtx_v  = plane_data._init_vtx_v;
    auto& scan_rect_v = plane_data._scan_rect_v;    
    auto& circle_scan_v = plane_data._circle_scan_v;

    // Initialize variables for this scan
    init_vtx_v.resize(init_vtx_v.size()+1);
    auto& init_vtx   = init_vtx_v.back();

    init_vtx.center = init_circle.center;

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
      init_vtx.xs_v.clear();
      for(size_t xs_idx=0; xs_idx<temp1_xs_v.size(); ++xs_idx)
	init_vtx.xs_v.push_back(data::PointPCA(temp1_xs_v[xs_idx],temp1_pca_v[xs_idx]));
    }else{
      init_vtx.xs_v.clear();
      if(temp1_xs_v.size() > temp2_xs_v.size()) {
	for(size_t xs_idx=0; xs_idx<temp1_xs_v.size(); ++xs_idx)
	  init_vtx.xs_v.push_back(data::PointPCA(temp1_xs_v[xs_idx],temp1_pca_v[xs_idx]));
      }else{
	for(size_t xs_idx=0; xs_idx<temp2_xs_v.size(); ++xs_idx)
	  init_vtx.xs_v.push_back(data::PointPCA(temp1_xs_v[xs_idx],temp1_pca_v[xs_idx]));
      }
    }

    // Define the (x,y) region to traverse along initial pca
    geo2d::Vector<float> top_left(init_vtx.center.x - _global_bound_size,
				  init_vtx.center.y + _global_bound_size);
    geo2d::Vector<float> bottom_right(init_vtx.center.x + _global_bound_size,
				      init_vtx.center.y - _global_bound_size);
    
    
    geo2d::Rect bbox(top_left, bottom_right);

	      
    scan_rect_v.push_back(bbox);

    LAROCV_DEBUG() << "Plane " << plane << " scanning bbox:" << bbox << std::endl;
    // Now we loop over per-xs-on-init-circle, and find a candidate vertex per xs point
    for(size_t xs_idx=0; xs_idx < init_vtx.xs_v.size(); ++xs_idx){

      auto const& xs  = init_vtx.xs_v[xs_idx].pt;
      auto const& pca = init_vtx.xs_v[xs_idx].line;

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

	//we are stepping outside the image, move on
	if ( row >= img.rows or col >= img.cols) { step_pt += dir; continue; }
	
	size_t marker_index = row + col * img.rows;
	if(scan_marker[marker_index]) { step_pt += dir; continue; }

	
	scan_marker[marker_index] = true;
	
	q = (float)(img.at<unsigned char>(row,col));
	
	if(q < _pi_threshold) {
	  step_pt += dir;
	  continue;
	}

	auto trial_pt = this->MeanPixel(img,step_pt);

	auto circle = this->TwoPointInspection(img,trial_pt);
	if(circle.xs_v.empty()) {
	  step_pt+= dir;
	  continue;
	}
	
	LAROCV_DEBUG() << "    ... dtheta_sum = " << circle.sum_dtheta() << std::endl;
	circle_scan_v.emplace_back(circle);
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
    auto& data = AlgoData<data::Refine2DVertexData>();

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

    if(_origin_v.size() <= meta.plane())
      _origin_v.resize(meta.plane()+1);
    _origin_v[meta.plane()].x = meta.origin().x;
    _origin_v[meta.plane()].y = meta.origin().y;

    geo2d::VectorArray<float> pt_v;
    geo2d::VectorArray<float> pt_err_v;
    geo2d::Vector<float> pt_err(2.,2.);

    bool found=false;
    geo2d::Circle<float> circle;
    circle.radius = _radius;

    if(_defect_algo_id != kINVALID_ID) {
      auto const& defect_pts = AlgoData<data::DefectClusterData>(_defect_algo_id);
      for(auto const& compound : defect_pts._raw_cluster_vv[meta.plane()].get_cluster()) {
	for(auto const& defect_pt : compound.get_defects()) {
	  const auto pt = defect_pt._pt_defect;
	  LAROCV_INFO() << "Scanning Defect point: " << pt << std::endl;
	  circle.center = pt;
	  found = PlaneScan(img,meta.plane(),circle,pt_err) || found;
	}
      }
    }

    if(_pca_algo_id != kINVALID_ID) {
      auto const& pca_pts    = AlgoData<data::PCACandidatesData>(_pca_algo_id);    
      for(auto const& pt : pca_pts.points(meta.plane())) {
	LAROCV_INFO() << "Scanning PCACandidate point: " << pt << std::endl;
	circle.center = pt;
	found = PlaneScan(img,meta.plane(),circle,pt_err) || found;
      }
    }
    
    if(!found) {
      data.get_plane_data_writeable(meta.plane())._valid_plane = false;
      return;
    }

    data.get_plane_data_writeable(meta.plane())._valid_plane = true;

  }

  void Refine2DVertex::XPlaneTimeScan(const std::vector<const cv::Mat>& img_v)
  {
    // Combine 3 plane information and make the best guesses as to which time tick may contain vertex(es)
    // To do this, we loop over time tick over all available planes first
    auto& data = AlgoData<data::Refine2DVertexData>();
    // Initialize results
    for(size_t plane=0; plane<img_v.size(); ++plane)
      _time_binned_score_v.clear();

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
    _xplane_tick_min = -1;
    _xplane_tick_max = -1;
    for(size_t plane=0; plane<img_v.size(); ++plane){
      auto const& plane_data = data.get_plane_data(plane);
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
	if(_xplane_tick_min < 0) _xplane_tick_min = ref_x;
	else if(_xplane_tick_min > ref_x) _xplane_tick_min = ref_x;
	if(_xplane_tick_max < 0) _xplane_tick_max = ref_x + bbox.width;
	else if(_xplane_tick_max < (ref_x + bbox.width)) _xplane_tick_max = ref_x + bbox.width;
      }
    }
    
    // Next define score map within a pre-defined tick resolution
    const size_t num_ticks = (_xplane_tick_max - _xplane_tick_min)/_xplane_tick_resolution + 1;
    LAROCV_DEBUG() << "NumTicks to scan in time: " << num_ticks << " (" << _xplane_tick_min << " => " << _xplane_tick_max << ")" << std::endl;

    for(size_t plane=0; plane<img_v.size(); ++plane) {
      _time_binned_score_v.resize(num_ticks+1);
      for(size_t tick=0; tick<=num_ticks; ++tick)
	_time_binned_score_v[tick] = -1;
    }
     
    for(size_t plane=0; plane<img_v.size(); ++plane) {
      auto& plane_data = data.get_plane_data_writeable(plane);
      //std::cout<<"Plane " << plane << " ..."<<std::endl;
      if(!plane_data._valid_plane) continue;
      auto const& circle_scan_v  = plane_data._circle_scan_v;
      auto const& tick_offset    = tick_offset_v.at(plane);
      
      for(size_t step=0; step<circle_scan_v.size(); ++step) {
	auto const& circle = circle_scan_v[step];
	auto const  dtheta = circle.sum_dtheta();
	float tick = circle.center.x - tick_offset;
	size_t tick_idx = (size_t)((tick - _xplane_tick_min)/_xplane_tick_resolution + 0.5);
	if(tick < _xplane_tick_min || tick > _xplane_tick_max) {
	  LAROCV_DEBUG() << "tick @ " << tick << " (array idx=" << tick_idx << ") .... dtheta @ " << dtheta <<  " ... skipping (out-o-range) " << std::endl;
	  continue;
	}

	auto& time_binned_score = _time_binned_score_v[tick_idx];
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

    _time_binned_score0_v.clear();
    _time_binned_score0_v.resize(num_ticks+1,-1);
    _time_binned_score1_v.clear();
    _time_binned_score1_v.resize(num_ticks+1,-1);
    for(size_t tick=0; tick < num_ticks; ++tick) {
      num_valid_planes = 0;
      dtheta_sum = 0.;
      LAROCV_DEBUG() << "At tick " << tick << "/" << num_ticks << std::endl;
      for(size_t plane=0; plane < img_v.size(); ++plane) {
	auto const& plane_data = data.get_plane_data(plane);
	if(!plane_data._valid_plane) continue;

	LAROCV_DEBUG() << "    Plane " << plane << " dtheta = " << _time_binned_score_v[tick] << std::endl;
	if(_time_binned_score_v[tick] < 0) continue;
	dtheta_sum += _time_binned_score_v[tick];
	num_valid_planes +=1;
      }
      LAROCV_DEBUG() << "    # valid plane = " << num_valid_planes << " dtheta = " << dtheta_sum << std::endl;

      // compute average dtheta_sum
      dtheta_sum /= (float)(num_valid_planes);

      if(num_valid_planes < 1) continue;
      _time_binned_score0_v[tick] = dtheta_sum;

      if(num_valid_planes < 2) continue;
      _time_binned_score1_v[tick] = dtheta_sum;
    }

    return;
  }

  void Refine2DVertex::XPlaneTickProposal()
  {
    //auto& data = AlgoData<larocv::Refine2DVertexData>();
    // Find local minimum for >2 plane score array
    auto const& score0_v  = _time_binned_score0_v;
    auto& mean_score0_v   = _time_binned_score0_mean_v;
    auto& minidx_score0_v = _time_binned_score0_minidx_v;
    mean_score0_v   = RollingMean(score0_v,3,3,-1);
    minidx_score0_v = ExtremePoints(mean_score0_v,5,5,true,-1);

    auto const& score1_v  = _time_binned_score1_v;
    auto& mean_score1_v   = _time_binned_score1_mean_v;
    auto& minidx_score1_v = _time_binned_score1_minidx_v;
    mean_score1_v   = RollingMean(score1_v,3,3,-1);
    minidx_score1_v = ExtremePoints(mean_score1_v,5,5,true,-1);

    // Now work on excluding neighbors for local minimum tick estimation
    auto& minidx_v = _time_binned_minidx_v;
    std::vector<bool> used_v;
    used_v.resize(minidx_score1_v.size(),false);
    
    for(size_t i=0; i<minidx_score1_v.size(); ++i) {
      auto const& central_idx = minidx_score1_v[i];
      if(used_v[i]) continue;
      
      int min_idx = (int)(central_idx) - _time_exclusion_radius;
      int max_idx = (int)(central_idx) + _time_exclusion_radius;
      if(min_idx<=0) min_idx = 0;
      if(max_idx>=mean_score1_v.size()) max_idx = mean_score1_v.size() - 1;
      LAROCV_DEBUG() << "Scanning 3-plane dtheta minimum around " << central_idx
		     << " in range [" << min_idx << "," << max_idx << "]" << std::endl;
      // method 1: find true minimum using dtheta
      float  min_score=1e9;
      size_t target_idx=kINVALID_SIZE;
      for(size_t idx=min_idx; idx<=max_idx; ++idx) {
	if(mean_score1_v[idx]<0) continue;
	if(target_idx == kINVALID_SIZE || mean_score1_v[idx] < min_score) {
	  target_idx = idx;
	  min_score  = mean_score1_v[idx];
	}
      }
      if(target_idx == kINVALID_SIZE) continue;

      // method 2: find minimum in terms of average of neighbor local minima
      /*
      float average_idx=central_idx;
      float average_ctr=1;
      for(size_t j=0; j<minidx_score1_v.size(); ++j) {
	if(used_v[j]) continue;
	auto const& neighbor_idx = minidx_score1_v[j];
	if(neighbor_idx < min_idx || max_idx < neighbor_idx) continue;
	average_idx += neighbor_idx;
	used_v[j] = true;
	++average_ctr;
      }
      size_t target_idx = (size_t)(average_idx/average_ctr+0.5);
      */      
      // check if this minimum is not a neighbor of others
      bool valid=true;
      for(auto const& minidx : minidx_v) {
	int dist = std::abs((int)(target_idx) - (int)(minidx));
	if(dist < _time_exclusion_radius) {
	  valid=false;
	  break;
	}
      }
      if(valid && target_idx != kINVALID_SIZE) {
	minidx_v.push_back(target_idx);
	LAROCV_INFO() << "Found valid 3-plane tick @ " << target_idx
		      << " neargy found local minimum dtheta minimum around " << central_idx << std::endl;
      }
    }

    used_v.clear();
    used_v.resize(minidx_score0_v.size(),false);
    for(size_t i=0; i<minidx_score0_v.size(); ++i) {
      auto const& central_idx = minidx_score0_v[i];
      if(used_v[i]) continue;

      int min_idx = (int)(central_idx) - _time_exclusion_radius;
      int max_idx = (int)(central_idx) + _time_exclusion_radius;
      if(min_idx<=0) min_idx = 0;
      if(max_idx>=mean_score0_v.size()) max_idx = mean_score0_v.size() - 1;
      LAROCV_DEBUG() << "Scanning 3-plane dtheta minimum around " << central_idx
		     << " in range [" << min_idx << "," << max_idx << "]" << std::endl;

      // method 1 find true minimum
      /*
      float  min_score=1e9;
      size_t target_idx=kINVALID_SIZE;
      for(size_t idx=min_idx; idx<=max_idx; ++idx) {
	if(mean_score1_v[idx]>=0) continue;
	if(mean_score0_v[idx]<0) continue;
	if(target_idx == kINVALID_SIZE || mean_score0_v[idx] < min_score) {
	  target_idx = idx;
	  min_score  = mean_score0_v[idx];
	}
      }
      if(target_idx == kINVALID_SIZE) continue;
      */
      // method 2: find minimum in terms of average of neighbor local minima
      float average_idx=central_idx;
      float average_ctr=1;
      for(size_t j=0; j<minidx_score0_v.size(); ++j) {
	if(used_v[j]) continue;
	auto const& neighbor_idx = minidx_score0_v[j];
	if(neighbor_idx < min_idx || max_idx < neighbor_idx) continue;
	average_idx += neighbor_idx;
	used_v[j] = true;
	++average_ctr;
      }
      size_t target_idx = (size_t)(average_idx/average_ctr+0.5);
      // check if this minimum is not a neighbor of others
      bool valid=true;
      for(auto const& minidx : minidx_v) {
	int dist = std::abs((int)(target_idx) - (int)(minidx));
	if(dist < _time_exclusion_radius) {
	  valid=false;
	  break;
	}
      }
      if(valid && target_idx != kINVALID_SIZE) {
	minidx_v.push_back(target_idx);
	LAROCV_INFO() << "Found valid 3-plane tick @ " << target_idx
		      << " neargy found local minimum dtheta minimum around " << central_idx << std::endl;
      }
    }
  }

  void Refine2DVertex::XPlaneWireScan(const std::vector<const cv::Mat>& img_v)
  {
    auto& data = AlgoData<data::Refine2DVertexData>();
    // Find candidate circles nearby proposed ticks
    for(auto const& binned_tick_idx : _time_binned_minidx_v) {

      const float target_tick = binned_tick_idx * _xplane_tick_resolution + _xplane_tick_min;

      LAROCV_DEBUG() << "Scanning wire space for vertex tick " << target_tick << std::endl;

      // Find two valid seed planes to construct 3D vertex hypothesis
      std::vector<size_t> valid_plane_v;
      std::vector<std::vector<size_t> > seed_circle_idx_v(img_v.size());
      
      for(auto const& plane : _seed_plane_v) {
	if(plane >= img_v.size()) {
	  LAROCV_CRITICAL() << "Invalid plane ID as a seed: " << plane << std::endl;
	  throw larbys();
	}

	auto const& plane_data = data.get_plane_data(plane);
	float dist=0;
	for(size_t circle_idx=0; circle_idx<plane_data._circle_scan_v.size(); ++circle_idx) {
	  auto const& circle = plane_data._circle_scan_v[circle_idx];
	  dist = std::fabs(circle.center.x - _tick_offset_v[plane] / _time_comp_factor_v[plane] - target_tick);
	  if(dist > (float)(_xplane_tick_resolution + 1.0)) continue;
	  seed_circle_idx_v[plane].push_back(circle_idx);
	}
	if(!seed_circle_idx_v[plane].empty()) {
	  LAROCV_DEBUG() << "Found seed plane " << plane << std::endl;
	  valid_plane_v.push_back(plane);
	}
      }

      if(valid_plane_v.size() < 2) continue;

      // Construct 3D vertex using 2+ plane
      std::set<size_t> seed0_used_idx_s;
      std::set<size_t> seed1_used_idx_s;
      std::vector<geo2d::VectorArray<float> > vertex_plane_vv(img_v.size());
      
      const size_t seed0_plane = valid_plane_v[0];
      const size_t seed1_plane = valid_plane_v[1];
      const size_t check_plane = ( valid_plane_v.size() > 2 ? valid_plane_v[2] : kINVALID_SIZE);
      LAROCV_INFO() << "Using two seed planes (" << seed0_plane << " and " << seed1_plane << ") "
		    << "to determine (y,z) vertex position" << std::endl;

      std::multimap<float,std::vector<size_t> > scoremap_2plane;
      std::multimap<float,std::vector<size_t> > scoremap_3plane;
      
      auto const& seed0_data = data.get_plane_data(seed0_plane);
      auto const& seed1_data = data.get_plane_data(seed1_plane);      
      // Construct all possible candidate vertexes
      for(auto const& circle0_idx : seed_circle_idx_v[seed0_plane]) {
	auto const& circle0 = seed0_data._circle_scan_v[circle0_idx];
	auto const  dtheta0 = circle0.sum_dtheta();
	for(auto const& circle1_idx : seed_circle_idx_v[seed1_plane]) {
	  auto const& circle1 = seed1_data._circle_scan_v[circle1_idx];
	  auto const  dtheta1 = circle1.sum_dtheta();
	  double y, z;
	  size_t wire0 = (size_t)(_origin_v[seed0_plane].y + circle0.center.y * _wire_comp_factor_v[seed0_plane] + 0.5);
	  size_t wire1 = (size_t)(_origin_v[seed1_plane].y + circle1.center.y * _wire_comp_factor_v[seed1_plane] + 0.5);
	  /*
	  LAROCV_DEBUG() << "Intersection pt1 ... plane " << seed0_plane
			 << " circle center @ " << circle0.center
			 << " image origin  @ " << _origin_v[seed0_plane]
			 << " wire comp factor = " << _wire_comp_factor_v[seed0_plane]
			 << " ... target wire = " << wire0 << std::endl;
	  LAROCV_DEBUG() << "Intersection pt2 ... plane " << seed1_plane
			 << " circle center @ " << circle1.center
			 << " image origin  @ " << _origin_v[seed1_plane]
			 << " wire comp factor = " << _wire_comp_factor_v[seed1_plane]
			 << " ... target wire = " << wire1 << std::endl;
	  */
	  try{
	    larocv::IntersectionPoint(wire0,seed0_plane,wire1,seed1_plane,y,z);
	  }catch(...){ continue; }

	  float ave_dtheta = dtheta0 + dtheta1;
	  /*
	  LAROCV_DEBUG() << "Vertex candidate (y,z) @ (" << y << "," << z << ") ... "
			 << "plane " << seed0_plane << " @ " << circle0.center << " ... "
			 << "plane " << seed1_plane << " @ " << circle1.center << std::endl;
	  */	  
	  size_t closest_circle2_idx = kINVALID_SIZE;
	  if(check_plane != kINVALID_SIZE) {
	    float  approx_wire2 = larocv::WireCoordinate(y,z,check_plane);
	    float  dist_min = 1e9;
	    auto const& check_data = data.get_plane_data(check_plane);
	    float  check_dtheta  = -1;
	    for(auto const& circle2_idx : seed_circle_idx_v[check_plane]) {
	      auto const& circle2 = check_data._circle_scan_v[circle2_idx];
	      auto const  dtheta2 = circle2.sum_dtheta();
	      float dist = std::fabs(_origin_v[check_plane].y + circle2.center.y * _wire_comp_factor_v[check_plane] - approx_wire2 );
	      if(dist < dist_min) {
		dist_min = dist;
		closest_circle2_idx = circle2_idx;
		check_dtheta = dtheta2;
	      }
	    }
	    if(dist_min > _xplane_wire_resolution) closest_circle2_idx = kINVALID_SIZE;
	    ave_dtheta += check_dtheta;
	  }
	  if(closest_circle2_idx != kINVALID_SIZE) {
	    ave_dtheta /= 3.;
	    std::vector<size_t> mapval(3);
	    mapval[0] = circle0_idx; mapval[1] = circle1_idx; mapval[2] = 3;
	    scoremap_3plane.emplace(ave_dtheta,mapval);
	    LAROCV_INFO() << "Found a 3-plane vertex candiate @ (y,z) = (" << y << "," << z << ") ... "
			  << "plane " << seed0_plane << " @ " << circle0.center << " ... "
			  << "plane " << seed1_plane << " @ " << circle1.center << " ... "
			  << "dtheta = " << ave_dtheta << std::endl;
	  }
	  else {
	    ave_dtheta /= 2.;
	    std::vector<size_t> mapval(3);
	    mapval[0] = circle0_idx; mapval[1] = circle1_idx; mapval[2] = 2;
	    scoremap_2plane.emplace(ave_dtheta,mapval);
	    LAROCV_INFO() << "Found a 2-plane vertex candiate @ (y,z) = (" << y << "," << z << ") ... "
			  << "plane " << seed0_plane << " @ " << circle0.center << " ... "
			  << "plane " << seed1_plane << " @ " << circle1.center << " ... "
			  << "dtheta = " << ave_dtheta << std::endl;
	  }
	}
      }

      // Now construct an array of possible candidates
      std::vector<std::vector<size_t> > seeds_v;
      seeds_v.reserve(scoremap_3plane.size()+scoremap_2plane.size());
      for(auto const& key_value : scoremap_3plane) seeds_v.push_back(key_value.second);
      if(!_require_3planes)
	for(auto const& key_value : scoremap_2plane) seeds_v.push_back(key_value.second);
      
      // loop over results and generate unique vertex set w/o neighbors
      geo2d::VectorArray<float> vtx_yz_v;
      for(auto const& seeds : seeds_v) {
	auto const& circle0_idx = seeds[0];
	auto const& circle1_idx = seeds[1];
	auto const& nplanes     = seeds[2];
	if(seed0_used_idx_s.find(circle0_idx) != seed0_used_idx_s.end()) continue;
	if(seed1_used_idx_s.find(circle0_idx) != seed1_used_idx_s.end()) continue;
	auto const& circle0 = seed0_data._circle_scan_v[circle0_idx];
	auto const& circle1 = seed1_data._circle_scan_v[circle1_idx];
	double y,z;
	size_t wire0 = (size_t)(_origin_v[seed0_plane].y + circle0.center.y * _wire_comp_factor_v[seed0_plane] + 0.5);
	size_t wire1 = (size_t)(_origin_v[seed1_plane].y + circle1.center.y * _wire_comp_factor_v[seed1_plane] + 0.5);
	larocv::IntersectionPoint(wire0,seed0_plane,wire1,seed1_plane,y,z);
	// Check if this vertex can be taken as a unique one w/o neighbors
	bool neighbor=false;
	geo2d::Vector<float> yz_pt(y,z);
	for(auto const& pt : vtx_yz_v) {
	  if(geo2d::dist(pt,yz_pt) < _vtx_3d_resolution) {
	    neighbor = true;
	    break;
	  }
	}
	if(neighbor) continue;

	// Concrete vertex found
	LAROCV_INFO() << "Claiming candiate @ (y,z) = (" << y << "," << z << ") ... "
		      << "plane " << seed0_plane << " @ " << circle0.center << " ... "
		      << "plane " << seed1_plane << " @ " << circle1.center << std::endl;
	
	vtx_yz_v.emplace_back(yz_pt);
	seed0_used_idx_s.insert(circle0_idx);
	seed1_used_idx_s.insert(circle1_idx);

	// Construct & fill 3D vertex container
	data::Vertex3D vtx3d;
	vtx3d.y = y;
	vtx3d.z = z;
	vtx3d.x = larocv::TriggerTick2Cm(circle0.center.x * _time_comp_factor_v[seed0_plane] +
					 _origin_v[seed0_plane].x - _trigger_tick);
	vtx3d.num_planes = nplanes;
	vtx3d.vtx2d_v.resize(img_v.size());
	vtx3d.vtx2d_v[seed0_plane].pt    = circle0.center;
	vtx3d.vtx2d_v[seed0_plane].score = circle0.sum_dtheta();
	vtx3d.vtx2d_v[seed1_plane].pt    = circle1.center;
	vtx3d.vtx2d_v[seed1_plane].score = circle1.sum_dtheta();
	
	// Construct & fill candidate 2D CircleVertex for seed planes
	std::vector<data::CircleVertex> circle_vtx_v(img_v.size());
	circle_vtx_v[seed0_plane] = circle0;
	circle_vtx_v[seed1_plane] = circle1;
	float approx_x = (circle0.center.x + circle1.center.x)/2.;
	// record other planes'
	for(size_t plane=0; plane<img_v.size(); ++plane) {	  
	  size_t closest_idx = kINVALID_SIZE;
	  float  approx_wire = larocv::WireCoordinate(y,z,plane);
	  float  approx_y = (approx_wire - _origin_v[plane].y) / _wire_comp_factor_v[plane];
	  float  dist_min = 1e9;
	  auto const& plane_data = data.get_plane_data(plane);
	  float  closest_dtheta = -1;
	  for(auto const& circle_idx : seed_circle_idx_v[plane]) {
	    auto const& circle = plane_data._circle_scan_v[circle_idx];
	    auto const  dtheta = circle.sum_dtheta();
	    float dist = std::fabs(approx_y - circle.center.y);
	    if(dist < dist_min) {
	      dist_min = dist;
	      closest_idx = circle_idx;
	      closest_dtheta = dtheta;
	    }
	  }
	  if(dist_min > _xplane_wire_resolution) closest_idx = kINVALID_SIZE;

	  // Fill candidate 2D CircleVertex for non-seed planes
	  if(closest_idx != kINVALID_SIZE)
	    circle_vtx_v[plane] = plane_data._circle_scan_v[closest_idx];
	  else {
	    geo2d::Vector<float> guess_pt;
	    guess_pt.x = approx_x;
	    guess_pt.y = approx_y;
	    auto guess_circle = TwoPointInspection(img_v[plane],guess_pt);
	    if(guess_circle.xs_v.empty()) {
	      guess_circle.center = guess_pt;
	      guess_circle.radius = _radius;
	    }
	    circle_vtx_v[plane] = guess_circle;
	  }
	  vtx3d.vtx2d_v[plane].pt.x  = approx_x;
	  vtx3d.vtx2d_v[plane].pt.y  = approx_y;
	  vtx3d.vtx2d_v[plane].score = circle_vtx_v[plane].sum_dtheta();
	  if(!vtx3d.vtx2d_v[plane].score) vtx3d.vtx2d_v[plane].score = -1;
	}
	data.emplace_back(std::move(vtx3d),std::move(circle_vtx_v));
      }
    }
  }

  bool Refine2DVertex::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    if(img_v.size() != _seed_plane_v.size()) {
      LAROCV_CRITICAL() << "seed_plane_v size != # planges ..." << std::endl;
      throw larbys();
    }
        
    _xplane_tick_min = _xplane_tick_max = -1;
    _time_binned_score0_v.clear();
    _time_binned_score0_mean_v.clear();
    _time_binned_score0_minidx_v.clear();
    _time_binned_score1_v.clear();
    _time_binned_score1_mean_v.clear();
    _time_binned_score1_minidx_v.clear();
    _time_binned_minidx_v.clear();

    XPlaneTimeScan(img_v);
    XPlaneTickProposal();
    XPlaneWireScan(img_v);
    return true;
  }

}
#endif
