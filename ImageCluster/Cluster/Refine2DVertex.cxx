#ifndef __REFINE2DVERTEX_CXX__
#define __REFINE2DVERTEX_CXX__

#include "Refine2DVertex.h"
#include "CircleVertexData.h"
#include "Core/HalfLine.h"
#include "Core/Line.h"
#include "Core/BoundingBox.h"

#include <array>
#include <map>
namespace larocv{

  /// Global larocv::Refine2DVertexFactory to register ClusterAlgoFactory
  static Refine2DVertexFactory __global_Refine2DVertexFactory__;

  void Refine2DVertex::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const circle_vtx_algo_name = pset.get<std::string>("CircleVertexAlgo");
    _circle_vertex_algo_id = this->ID(circle_vtx_algo_name);
    _radius = 15;
    _pi_threshold = 10;
    _pca_box_size = 3;
    _global_bound_size = 25;

    _tick_offset_v.resize(3);
    _tick_offset_v[0] = 0.;
    //_tick_offset_v[1] = 4.54;
    //_tick_offset_v[2] = 7.78;
    _tick_offset_v[1] = 0.;
    _tick_offset_v[2] = 0.;
    _use_polar_spread = false;
    _comp_factor_v.resize(3);
    
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
					       const size_t side)
  {
					       
    auto small_img = ::cv::Mat(img,::cv::Rect(pt.x-side, pt.y-side, 2*side+1, 2*side+1));
    ::cv::Mat thresh_small_img;
    //::cv::threshold(small_img,thresh_small_img,_pi_threshold,1,CV_THRESH_BINARY);
    ::cv::threshold(small_img,thresh_small_img,_pi_threshold,1000,3);
    geo2d::VectorArray<int> points;
    findNonZero(thresh_small_img, points);
    
    cv::Mat mat_points(points.size(), 2, CV_32FC1);
    for (unsigned i = 0; i < mat_points.rows; ++i) {
      mat_points.at<float>(i, 0) = points[i].x;
      mat_points.at<float>(i, 1) = points[i].y;
    }
    
    ::cv::PCA pcaobj(mat_points,::cv::Mat(),::cv::PCA::DATA_AS_ROW,0);
    
    pt.x += pcaobj.mean.at<float>(0,0) - side;
    pt.y += pcaobj.mean.at<float>(0,1) - side;
    
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

  void Refine2DVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
				 const ::cv::Mat& img,
				 larocv::ImageMeta& meta,
				 larocv::ROI& roi)
  {
    _comp_factor_v[meta.plane()] = meta.pixel_width();
    
    auto const& cvtx_data = AlgoData<larocv::CircleVertexData>(_circle_vertex_algo_id);
    auto const& circle_vv = cvtx_data._circledata_v_v;
    
    if(circle_vv.empty()) {
      LAROCV_CRITICAL() << "No plane? Failed to retrieve circles from CircleVertexAlgo" << std::endl;
      throw larbys();
    }
    auto const& circle_v = circle_vv[meta.plane()];

    if(circle_v.empty()) {
      LAROCV_DEBUG() << "No circle found: nothing to refine" << std::endl;
      throw larbys();
    }

    auto& data = AlgoData<larocv::Refine2DVertexData>();
    auto& xs_v = data._xs_vv[meta.plane()];
    auto& vtx  = data._vtx_v[meta.plane()];
    auto& pca_v = data._pca_vv[meta.plane()];
      
    vtx = circle_v.front().center;
    pca_v.clear();
    xs_v.clear();

    // Find xs point on the previously guessed circle
    xs_v = QPointOnCircle(img,circle_v.front());

    // Find PCAs per xs point 
    for(auto const& pt : xs_v)
      pca_v.push_back(SquarePCA(img,pt,_pca_box_size));

    // Define the (x,y) region to traverse along initial pca
    geo2d::Vector<float> top_left(vtx.x - _global_bound_size,
				  vtx.y + _global_bound_size);
    geo2d::Vector<float> bottom_right(vtx.x + _global_bound_size,
				      vtx.y - _global_bound_size);
    geo2d::Rect bbox(top_left, bottom_right);
    LAROCV_DEBUG() << "Plane " << meta.plane() << " scanning bbox:" << bbox << std::endl;
    // Now we loop over per-xs-on-init-circle, and find a candidate vertex per xs point
    //std::multimap< double, geo2d::Vector<float> > candidate_vtx_m;
    for(size_t xs_idx=0; xs_idx < xs_v.size(); ++xs_idx){

      auto const& xs  = xs_v [xs_idx];
      auto const& pca = pca_v[xs_idx];

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
      auto step_pt = start + dir;
      double min_dtheta_sum = 1e4;
      geo2d::Vector<float> candidate_vtx = start;
      
      while(bbox.contains(step_pt)) {

	// Check if this point has any charge. if not continue
	col = (size_t)(step_pt.x);
	row = (size_t)(step_pt.y);
	q = (float)(img.at<unsigned char>(row,col));

	//std::cout << start << " ... " << step_pt << ", " << q << std::endl;

	if(q < _pi_threshold) {
	  step_pt += dir;
	  continue;
	}

	// Make a better guess
	auto small_img = ::cv::Mat(img,::cv::Rect(step_pt.x - 2, step_pt.y - 2, 5, 5));
	::cv::Mat thresh_small_img;
	::cv::threshold(small_img,thresh_small_img,_pi_threshold,1,CV_THRESH_BINARY);
	//::cv::threshold(small_img,thresh_small_img,_pi_threshold,1000,3);

	geo2d::VectorArray<int> points;
	findNonZero(thresh_small_img, points);

	geo2d::Vector<float> trial_pt(0,0);
	for (unsigned i = 0; i < points.size(); ++i){
	  trial_pt.x += points[i].x;
	  trial_pt.y += points[i].y;
	}

	trial_pt.x /= (float)(points.size());
	trial_pt.y /= (float)(points.size());

	trial_pt.x = step_pt.x + trial_pt.x - 2;
	trial_pt.y = step_pt.y + trial_pt.y - 2;

	// Get xs points for this step. if < 2 continue
	auto const step_xs_pts = QPointOnCircle(img,geo2d::Circle<float>(trial_pt,_radius));

	//data._circle_trav_vv[meta.plane()].emplace_back(std::move(geo2d::Circle<float>(trial_pt,_radius)));
	
	if(step_xs_pts.size()<2) {
	  step_pt += dir;
	  continue;
	}
	
	if(step_xs_pts.size()==2) {
	  auto center_line0 = geo2d::Line<float>(step_xs_pts[0], step_xs_pts[0] - trial_pt);
	  auto center_line1 = geo2d::Line<float>(step_xs_pts[1], step_xs_pts[1] - trial_pt);
	  auto dtheta = fabs(geo2d::angle(center_line0) - geo2d::angle(center_line1));
	  if(dtheta < 10) {
	    step_pt+= dir;
	    continue;
	  }
	}

	double dtheta_sum = 0;
	if(_use_polar_spread) {
	  // Estimate a polar y-spread as am easure of "dtheta"
	  ::cv::Mat local_polarimg;
	  ::cv::linearPolar(img, local_polarimg, trial_pt, _radius+10., ::cv::WARP_FILL_OUTLIERS);
	  for(auto const& step_xs_pt : step_xs_pts) {
	    float angle = geo2d::angle(trial_pt,step_xs_pt);
	    float radius_frac_min = (_radius - 2.) / (_radius+10.);
	    float radius_frac_max = (_radius + 3.) / (_radius+10.);
	    dtheta_sum += AngularSpread(local_polarimg,radius_frac_min,radius_frac_max,angle,10.);
	  }
	}else{
	  // Estimate a local PCA and center-to-xs line's angle diff
	  for(auto const& step_xs_pt : step_xs_pts) {
	    // Line that connects center to xs
	    auto center_line = geo2d::Line<float>(step_xs_pt, step_xs_pt - trial_pt);
	    auto local_pca   = SquarePCA(img,step_xs_pt,_pca_box_size);
	    dtheta_sum += fabs(geo2d::angle(center_line) - geo2d::angle(local_pca));
	    // Alternative (and probably better/faster): compute y spread in polar coordinate
	    LAROCV_DEBUG() << "Line ID " << xs_idx << " found xs " << step_xs_pt << " dtheta " << fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)) << std::endl;
	  }
	}
	
	dtheta_sum /= (float)(step_xs_pts.size());
	LAROCV_DEBUG() << "    ... dtheta_sum = " << dtheta_sum << std::endl;
	data._circle_trav_vv[meta.plane()].emplace_back(std::move(geo2d::Circle<float>(trial_pt,_radius)));
	data._dtheta_trav_vv[meta.plane()].push_back(dtheta_sum);

	if(dtheta_sum < min_dtheta_sum) {
	  min_dtheta_sum = dtheta_sum;
	  candidate_vtx = trial_pt;
	}
	
	// Increment the step and continue or break
	step_pt += dir;	
      }

      // Fine tuning w/ neighbor pixels
      /*
      if(min_dtheta_sum < 1e4) {
	auto small_img = ::cv::Mat(img,::cv::Rect(candidate_vtx.x - 2, candidate_vtx.y - 2, 5, 5));
	::cv::Mat thresh_small_img;
	::cv::threshold(small_img,thresh_small_img,_pi_threshold,1,CV_THRESH_BINARY);

	geo2d::VectorArray<int> points;
	findNonZero(thresh_small_img, points);

	geo2d::Vector<float> mean_vtx(0,0);
	for (unsigned i = 0; i < points.size(); ++i){
	  mean_vtx.x += points[i].x;
	  mean_vtx.y += points[i].y;
	}

	mean_vtx.x /= (float)(points.size());
	mean_vtx.y /= (float)(points.size());

	candidate_vtx.x += (mean_vtx.x - 2);
	candidate_vtx.y += (mean_vtx.y - 2);
      }
      candidate_vtx_m.emplace(min_dtheta_sum,candidate_vtx);
      */
    }

    /*
    for(auto const& score_vtx : candidate_vtx_m) {
      data._cand_score_vv[meta.plane()].push_back(score_vtx.first);
      data._cand_vtx_vv[meta.plane()].push_back(score_vtx.second);      
    }
    */
    data._scan_rect_v[meta.plane()] = bbox;

  }

  void Refine2DVertex::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    // Combine 3 plane information and make a best guess.
    // To do this, we loop over time tick over all available planes
    auto& data = AlgoData<larocv::Refine2DVertexData>();

    // Compute tick offset per plane
    std::vector<float> tick_offset_v = _tick_offset_v;
    for(size_t plane=0; plane<data._scan_rect_v.size(); ++plane) {
      tick_offset_v[plane] /= 6.;//_comp_factor_v[plane];
      LAROCV_DEBUG() << "Plane " << plane
		     << " offset = " << tick_offset_v[plane]
		     << " @ compression " << _comp_factor_v[plane]
		     << std::endl;
    }

    // First obtain overlapping time tick regions
    float min_tick = -1;
    float max_tick = -1;
    for(size_t plane=0; plane<data._scan_rect_v.size(); ++plane){
      auto const& bbox = data._scan_rect_v[plane];
      float ref_x = bbox.x;
      ref_x -= tick_offset_v[plane];
      if(min_tick < 0) min_tick = ref_x;
      else if(min_tick < ref_x) min_tick = ref_x;
      if(max_tick < 0) max_tick = ref_x + bbox.width;
      else if(max_tick > (ref_x + bbox.width)) max_tick = ref_x + bbox.width;
    }

    // Next define score map within a tick resolution of 3
    const size_t num_ticks = (max_tick - min_tick)/1 + 1;
    LAROCV_DEBUG() << "NumTicks to scan in time: " << num_ticks << std::endl;

    std::vector< std::vector<std::array<float,3> > > vtx_list_vv;
    vtx_list_vv.resize(data._dtheta_trav_vv.size());
    for(auto& vtx_list_v : vtx_list_vv) {
      vtx_list_v.resize(num_ticks+1);
      for(auto& vtx_list : vtx_list_v) {
	vtx_list.at(0) = vtx_list.at(1) = vtx_list.at(2) = -1.;
      }
    }
    
    for(size_t plane=0; plane<data._dtheta_trav_vv.size(); ++plane) {
      std::cout<<"Plane " << plane << " ..."<<std::endl;
      auto const& dtheta_trav_v = data._dtheta_trav_vv.at(plane);
      auto const& circle_trav_v = data._circle_trav_vv.at(plane);
      auto const& tick_offset = tick_offset_v.at(plane);
      auto& vtx_list_v = vtx_list_vv.at(plane);
      for(size_t step=0; step<circle_trav_v.size(); ++step) {
	auto const& dtheta = dtheta_trav_v.at(step);
	auto const& circle = circle_trav_v.at(step);
	float tick = circle.center.x - tick_offset;
	if(tick < min_tick || tick > max_tick) {
	  LAROCV_DEBUG() << "tick @ " << tick << " (array idx=" << (float)((tick-min_tick)/1) << ") .... dtheta @ " << dtheta <<  " ... skipping (out-o-range) " << std::endl;
	  continue;
	}
	auto& vtx_list = vtx_list_v.at((size_t)((tick - min_tick)/1));
	if(vtx_list.at(0) > 0 && vtx_list.at(0) < dtheta) {
	  LAROCV_DEBUG() << "tick @ " << tick << " (array idx=" << (float)((tick-min_tick)/1) << ") .... dtheta @ " << dtheta <<  " ... skipping (dtheta too big) " << std::endl;
	  continue;
	}
	LAROCV_DEBUG() << "tick @ " << tick << " (array idx=" << (float)((tick-min_tick)/1) << ") .... dtheta @ " << dtheta <<  " ... registering " << std::endl;
	vtx_list.at(0) = dtheta;
	vtx_list.at(1) = circle.center.x;
	vtx_list.at(2) = circle.center.y;
      }
    }

    // Now loop over each "tick" and compute the best matching vertex point
    size_t num_valid_planes = 0;
    float  min_dtheta_sum = 1e4;
    float  dtheta_sum = 0.;
    size_t min_dtheta_sum_tick = kINVALID_SIZE;
    for(size_t tick=0; tick < num_ticks; ++tick) {
      num_valid_planes = 0;
      dtheta_sum = 0.;
      LAROCV_DEBUG() << "At tick " << tick << "/" << num_ticks << std::endl;
      for(size_t plane=0; plane < vtx_list_vv.size(); ++plane) {
	LAROCV_DEBUG() << "    Plane " << plane << " dtheta = " << vtx_list_vv[plane][tick][0] << std::endl;
	if(vtx_list_vv[plane][tick][0] < 0) continue;
	dtheta_sum += vtx_list_vv[plane][tick][0];
	num_valid_planes +=1;
      }
      LAROCV_DEBUG() << "    # valid plane = " << num_valid_planes << " dtheta = " << dtheta_sum << std::endl;
      if(num_valid_planes < 2) continue;
      // compute average dtheta_sum
      dtheta_sum /= (float)(num_valid_planes);
      if(dtheta_sum > min_dtheta_sum) continue;
      LAROCV_DEBUG() << "    ... new minimum!" << std::endl;
      min_dtheta_sum = dtheta_sum;
      min_dtheta_sum_tick = tick;
    }

    // Now record best vtx point per plane
    for(size_t plane=0; plane < vtx_list_vv.size(); ++plane) {
      auto const& best_vtx = vtx_list_vv[plane][min_dtheta_sum_tick];
      data._cand_vtx_v[plane] = geo2d::Vector<float>(best_vtx[1],best_vtx[2]);
      data._cand_score_v[plane] = best_vtx[0];

      // Obtain crossing point w/ a circle around the vertex. These are particle cluster candidates.
      geo2d::Circle<float> circle;
      circle.center.x = best_vtx[1];
      circle.center.y = best_vtx[2];
      circle.radius = _radius;
      data._cand_xs_vv[plane] = QPointOnCircle(img_v[plane],circle);
    }

  }

}
#endif
