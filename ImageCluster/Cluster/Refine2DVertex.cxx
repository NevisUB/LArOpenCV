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

    _trigger_tick = 3200; // neutrino triggers @ 3200 tick

    _straight_line_angle_cut = pset.get<float>("StraightLineAngleCut",10);

    _radius = pset.get<float>("CircleVScanRadius",6);

    _cvtx_min_radius  = pset.get<double>("CircleVMinRadius",4);
    _cvtx_max_radius  = pset.get<double>("CircleVMaxRadius",21);
    _cvtx_radius_step = pset.get<double>("CircleVMinRadius",2);
    _pi_threshold = pset.get<float>("PIThreshold",10);
    _pca_box_size = pset.get<float>("PCABoxSize",3);
    _global_bound_size = pset.get<float>("GlobalBoundSize",20);

    _clean_image = pset.get<bool>("CleanImage",false);

    _tick_offset_v.resize(3);
    _tick_offset_v[0] = 0.;
    _tick_offset_v[1] = 4.54;
    _tick_offset_v[2] = 7.78;
    //_tick_offset_v[1] = 0.;
    //_tick_offset_v[2] = 0.;
    _use_polar_spread = pset.get<bool>("UsePolarSpread",false);
    _wire_comp_factor_v.resize(3);
    _time_comp_factor_v.resize(3);
    _xplane_tick_resolution = pset.get<float>("TickResolution",1);
    _xplane_wire_resolution = pset.get<float>("WireResolution",2.5);
    _xplane_guess_max_dist  = pset.get<float>("GuessMaxDist",3);
    _time_exclusion_radius  = pset.get<float>("TimeExclusionRadius",10);
    _wire_exclusion_radius  = pset.get<float>("WireExclusionRadius",10);

    _require_3planes = pset.get<bool>("Require3Planes",false);
    _vtx_3d_resolution = pset.get<float>("Vtx3DResolution",10);

    _seed_plane_v.resize(3);
    _seed_plane_v[0] = 2;
    _seed_plane_v[1] = 0;
    _seed_plane_v[2] = 1;

    _min_contour_length = 5;
    _min_contour_rect_area = 20;
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
      //if(idx < pre) continue;
      //if(idx+post >= array.size()) continue;

      // Compute average in pre sample
      local_sum = 0;
      valid_ctr = 0;
      if(idx >= pre) {
	for(size_t subidx=(idx-pre); subidx < idx; ++subidx){
	  if(array[subidx] == ignore_value) continue;
	  local_sum += array[subidx];
	  ++valid_ctr;
	}
	if(valid_ctr < 2) continue;
	pre_mean = local_sum / (float)(valid_ctr);
      }else if(array[idx] != ignore_value) {
	pre_mean = array[idx];
      }else
	continue;
      
      // Compute average in post sample
      local_sum = 0;
      valid_ctr = 0;
      if(idx+post < array.size()) {
	for(size_t subidx=idx+1; subidx <= idx+post; ++subidx){
	  if(array[subidx] == ignore_value) continue;
	  local_sum += array[subidx];
	  ++valid_ctr;
	}
	if(valid_ctr < 2) continue;
	post_mean = local_sum / (float)(valid_ctr);
      }else if(array.back() != ignore_value) {
	post_mean = array.back();
      }else
	continue;
      
      slope[idx] = (post_mean - pre_mean) / (((float)pre)/2. + ((float)post)/2. + 1.);
    }
    return slope;
  }

  void Refine2DVertex::ExtremePoints(const std::vector<float>& array,
				     size_t pre, size_t post, bool minimum, bool inspect_edges,
				     std::vector<size_t>& local_extreme_idx_v,
				     std::vector<std::pair<size_t,size_t> >& local_extreme_range_v,
				     float invalid_value)
  {
    local_extreme_idx_v.clear();
    local_extreme_range_v.clear();

    std::vector<size_t> temp_extreme_idx_v;
    std::vector<std::pair<size_t,size_t> > temp_extreme_range_v;
    
    auto const& slope = RollingGradient(array,pre,post,invalid_value);
    float preval, postval;
    int prestep, poststep;
    bool found;
    // First find the "bottom of a valley", a local minimum
    for(size_t idx=0; idx<array.size(); ++idx) {
      //std::cout << "dtheta: " << array[idx] << " slope: " << slope[idx] << " @ idx " << idx << std::endl;
      if(array[idx] == invalid_value) continue;
      if(slope[idx] == invalid_value) continue;
      
      preval  = 0.;
      prestep = idx-1;
      found = false;
      // Find a pre-step that holds the "previous slope"
      while(prestep>=0 && prestep>((int)idx - pre - 1)) {
	if(slope[prestep]==invalid_value) {
	  prestep -=1;
	  continue;
	}
	preval = slope[prestep];
	found=true;
	break;
      }
      // If no "previous slope" found, ignore this tick
      if(!found) continue;
      
      postval = 0.;
      poststep = idx+1;
      found=false;
      // Find a post-step that holds the "later slope"
      while(poststep<=slope.size() && poststep < ((int)idx + post +1)) {
	if(slope[poststep]==invalid_value) {
	  poststep +=1;
	  continue;
	}
	postval = slope[poststep];
	found=true;
	break;
      }
      // If no "later slope" found, ignore this tick
      if(!found) continue;

      if(  minimum && preval < 0 and postval > 0 ) temp_extreme_idx_v.push_back(idx);
      if( !minimum && preval > 0 and postval < 0 ) temp_extreme_idx_v.push_back(idx);
    }

    // Now figure out range of "valley"
    temp_extreme_range_v.resize(temp_extreme_idx_v.size());
    for(size_t minima_idx = 0; minima_idx < temp_extreme_idx_v.size(); ++minima_idx) {
      auto const& minima_candidate = temp_extreme_idx_v[minima_idx];

      // target range to be figured out
      size_t range_start = minima_candidate;
      size_t range_end   = minima_candidate;
      // loop control variables
      size_t next_idx, consecutive_invalid_count;

      consecutive_invalid_count = 0;
      while(range_start > consecutive_invalid_count && consecutive_invalid_count < pre) {
	next_idx = range_start - consecutive_invalid_count - 1 ;
	auto const& local_slope = slope[next_idx];
	if(local_slope != invalid_value) consecutive_invalid_count = 0;
	else {
	  ++consecutive_invalid_count;
	  continue;
	}
	if( (minimum && local_slope>=0) || (!minimum && local_slope<=0) ) break;
	range_start = next_idx;
      }

      consecutive_invalid_count = 0;
      while( (range_end+consecutive_invalid_count) < slope.size() && consecutive_invalid_count < pre) {
	next_idx = range_end + consecutive_invalid_count + 1 ;
	auto const& local_slope = slope[next_idx];
	if(local_slope != invalid_value) consecutive_invalid_count = 0;
	else {
	  ++consecutive_invalid_count;
	  continue;
	}
	if( (minimum && local_slope<=0) || (!minimum && local_slope>=0) ) break;
	range_end = next_idx;
      }
      
      temp_extreme_range_v[minima_idx] = std::make_pair(range_start,range_end);
    }

    // If the range does not cover the start/end of array, and if a user specified to check, check.
    if(inspect_edges) {
      // get first and last valid index
      size_t first_valid_index=kINVALID_SIZE;
      size_t last_valid_index=kINVALID_SIZE;
      for(size_t idx=0; idx<array.size(); ++idx) {
	if(array[idx] == invalid_value) continue;
	first_valid_index = idx;
	break;
      }
      for(size_t idx=0; idx<array.size(); ++idx) {
	if(array[array.size()-idx-1] == invalid_value) continue;
	last_valid_index = array.size() - idx - 1;
	break;
      }
      // check the start: use 2*pre range to check if the beginning has a slope w/ same sign.
      // if that's the case it is possible that the edge is a true extreme
      bool start_included=false;
      for(auto const& range : temp_extreme_range_v) {
	if(range.first <= first_valid_index && first_valid_index <= range.second) {
	  start_included = true;
	  break;
	}
      }
      if(!start_included && first_valid_index != kINVALID_SIZE && (first_valid_index+pre*2) <= array.size()) {
	bool include_start = true;
	size_t num_index = 0;
	while(num_index<(pre*2)) {
	  if(array[first_valid_index + num_index] == invalid_value) {
	    include_start = false;
	    break;
	  }
	  if( slope[first_valid_index + num_index] == invalid_value ) {
	    ++num_index;
	    continue;
	  }
	  if(  minimum && slope[first_valid_index + num_index] <= 0 ) {
	    include_start = false;
	    break;
	  }
	  if( !minimum && slope[first_valid_index + num_index] >= 0 ) {
	    include_start = false;
	    break;
	  }
	  ++num_index;
	}
	if(include_start) {
	  temp_extreme_range_v.push_back(std::make_pair((size_t)(first_valid_index),(size_t)(first_valid_index + pre*2)));
	}
      }
      // check the end: use 2*post range to check if the ending has a slope w/ same sign.
      // if that's the case it is possible that the edge is a true extreme
      bool end_included=false;
      for(auto const& range : temp_extreme_range_v) {
	if(range.first <= last_valid_index && last_valid_index <= range.second) {
	  end_included = true;
	  break;
	}
      }
      if(!end_included && last_valid_index != kINVALID_SIZE && array.size() > (post*2)) {
	bool include_end = true;
	size_t num_index = 0;
	while(num_index<(post*2)) {
	  if(array[last_valid_index - num_index] == invalid_value) {
	    include_end = false;
	    break;
	  }
	  if( slope[last_valid_index - num_index] == invalid_value ) {
	    ++num_index;
	    continue;
	  }
	  if(  minimum && slope[last_valid_index - num_index] >= 0 ) {
	    include_end = false;
	    break;
	  }
	  if( !minimum && slope[last_valid_index - num_index] <= 0 ) {
	    include_end = false;
	    break;
	  }
	  ++num_index;
	}
	if(include_end)
	  temp_extreme_range_v.push_back(std::make_pair((size_t)(last_valid_index-post*2+1),(size_t)(last_valid_index)));
      }
    }

    // Now choose the unique range
    local_extreme_range_v.reserve(temp_extreme_range_v.size());
    for(auto const& temp_range : temp_extreme_range_v) {
      auto const& start = temp_range.first;
      auto const& end   = temp_range.second;
      // check if there is an overlap with constructed range
      bool overlap=false;
      for(auto& range : local_extreme_range_v) {
	if(end < range.first || range.second < start) continue;
	overlap=true;
	range.first  = (range.first  < start ? range.first  : start );
	range.second = (range.second > end   ? range.second : end   );
	break;
      }
      if(!overlap) local_extreme_range_v.push_back(temp_range);
    }
        
    // Now find local minima within the unique range
    for(auto const& range : local_extreme_range_v) {
      auto const& start = range.first;
      auto const& end   = range.second;
      float extreme_val = 1e9;
      size_t extreme_idx = kINVALID_SIZE;
      if(!minimum) extreme_val = 0;
      
      for(size_t idx=start; idx<=end; ++idx) {
	if(array[idx] == invalid_value) continue;
	if(minimum && array[idx] < extreme_val) {
	  extreme_idx = idx;
	  extreme_val = array[idx];
	}
	if(!minimum && array[idx] > extreme_val) {
	  extreme_idx = idx;
	  extreme_val = array[idx];
	}
      }
      local_extreme_idx_v.push_back(extreme_idx);
    }
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

  void Refine2DVertex::FindEdges(const GEO2D_Contour_t& ctor,
				 geo2d::Vector<float>& edge1,
				 geo2d::Vector<float>& edge2) const
  {
    // cheap trick assuming this is a linear, linear track cluster
    geo2d::Vector<float> mean_pt, ctor_pt;
    mean_pt.x = mean_pt.y = 0.;
    for(auto const& pt : ctor) { mean_pt.x += pt.x; mean_pt.y += pt.y; }
    mean_pt.x /= (double)(ctor.size());
    mean_pt.y /= (double)(ctor.size());
    // find the furthest point from the mean (x,y)
    double dist_max=0;
    double dist;
    for(auto const& pt : ctor) {
      ctor_pt.x = pt.x;
      ctor_pt.y = pt.y;
      dist = geo2d::dist(mean_pt,ctor_pt);
      if(dist > dist_max) {
	edge1 = pt;
	dist_max = dist;
      }
    }
    // find the furthest point from edge1
    dist_max=0;
    for(auto const& pt : ctor) {
      ctor_pt.x = pt.x;
      ctor_pt.y = pt.y;
      dist = geo2d::dist(edge1,ctor_pt);
      if(dist > dist_max) {
	edge2 = pt;
	dist_max = dist;
      }
    }
  }

  cv::Mat Refine2DVertex::CleanImage(const cv::Mat& img,
				     const GEO2D_ContourArray_t& veto_ctor_v) const
  {
    cv::Mat thresh_img;
    ::cv::threshold(img,thresh_img,_pi_threshold,1000,3);
    geo2d::VectorArray<int> points;
    findNonZero(thresh_img, points);
    for(auto const& pt : points) {
      for(auto const& veto_ctor : veto_ctor_v) {
	auto dist = ::cv::pointPolygonTest(veto_ctor,pt,false);
	if(dist<0) continue;
	thresh_img.at<unsigned char>(pt.y,pt.x) = 0;
	break;
      }
    }
    return thresh_img;
  }
  
  std::vector<std::vector<geo2d::Vector<int> > >
  Refine2DVertex::VertexVetoRegion(const ::cv::Mat& img)
  {
    // threshold
    cv::Mat thresh_img;
    ::cv::threshold(img,thresh_img,_pi_threshold,1,CV_THRESH_BINARY);
    // find contours
    GEO2D_ContourArray_t ctor_v;
    std::vector<::cv::Vec4i> cv_hierarchy_v;
    ::cv::findContours(thresh_img, ctor_v, cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    // select too-small contours
    GEO2D_ContourArray_t res_v;
    res_v.reserve(ctor_v.size());
    geo2d::Vector<float> edge1, edge2;
    for(auto const& ctor : ctor_v) {
      //rectangle area
      auto rrect = cv::minAreaRect(ctor);
      auto rrect_area = rrect.size.area();
      //contour length
      FindEdges(ctor,edge1,edge2);
      auto length = geo2d::dist(edge1,edge2);
      if(length < _min_contour_length ||
	 rrect_area < _min_contour_rect_area) {
	res_v.push_back(ctor);
      }
    }
    return res_v;
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

    //std::cout << "Mean @ (" << pt.x << "," << pt.y << ") ... dir (" << dir.x << "," << dir.y << std::endl;

    return geo2d::Line<float>(pt,dir);
  }

  geo2d::VectorArray<float> Refine2DVertex::QPointOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle)
  {
    geo2d::VectorArray<float> res;
    // Find crossing point
    ::cv::Mat polarimg;
    ::cv::linearPolar(img, polarimg, circle.center, circle.radius*2, ::cv::WARP_FILL_OUTLIERS);
    
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
      pt.x = circle.center.x + circle.radius * cos(angle);
      pt.y = circle.center.y + circle.radius * sin(angle);

      res.push_back(pt);
    }
    return res;
  }

  std::vector<geo2d::VectorArray<float> >
  Refine2DVertex::QPointArrayOnCircleArray(const ::cv::Mat& img, const geo2d::Vector<float>& center,
					   const std::vector<float>& radius_v) const
  {
    std::vector<geo2d::VectorArray<float> > res_v;
    if(radius_v.empty()) return res_v;
    float max_radi = 0;
    float min_radi = -1;
    for(auto const& r : radius_v) {
      if(r<=0) throw larbys("non-positive radius cannot be processed!");
      if(min_radi<0) { min_radi = max_radi = r; continue; }
      if(max_radi < r) max_radi = r;
      if(min_radi > r) min_radi = r;
    }
    if(max_radi == min_radi) max_radi *= 1.1;
    else max_radi += (max_radi - min_radi);
    
    // Find crossing point
    ::cv::Mat polarimg;
    ::cv::linearPolar(img, polarimg, center, max_radi, ::cv::WARP_FILL_OUTLIERS);

    std::vector<size_t> col_v(radius_v.size(),0);
    for(size_t r_idx=0; r_idx<radius_v.size(); ++r_idx) {
      auto const& radius = radius_v[r_idx];
      col_v[r_idx] = (size_t)(radius / max_radi * (float)(polarimg.cols) + 0.5);
    }

    for(size_t col_idx=0; col_idx<col_v.size(); ++col_idx) {

      auto const& col    = col_v[col_idx];
      auto const& radius = radius_v[col_idx];

      geo2d::VectorArray<float> res;
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
	pt.x = center.x + radius * cos(angle);
	pt.y = center.y + radius * sin(angle);
	
	res.push_back(pt);      
      }

      res_v.emplace_back(std::move(res));
    }
    return res_v;
  }

  void Refine2DVertex::edge_rect(const ::cv::Mat& img, cv::Rect& rect,int w, int h) const
  {

    //make it edge aware
    if ( rect.x < 0 ) rect.x = 0;
    if ( rect.y < 0 ) rect.y = 0;
    
    if ( rect.x > img.cols - w ) rect.x = img.cols - w;
    if ( rect.y > img.rows - h ) rect.y = img.rows - h;
    
  }
  
  geo2d::Vector<float> Refine2DVertex::MeanPixel(const cv::Mat& img,
						 const geo2d::Vector<float>& center,
						 size_t range_x, size_t range_y) const
  {
    // Make a better guess
    ::cv::Rect rect(center.x - range_x, center.y - range_y, range_x*2+1,range_y*2+1);
    edge_rect(img,rect,range_x*2+1,range_y*2+1);
    
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
    
    trial_pt.x = center.x + trial_pt.x - range_x;
    trial_pt.y = center.y + trial_pt.y - range_y;

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

  data::CircleVertex Refine2DVertex::RadialScan(const cv::Mat& img, const geo2d::Vector<float>& pt)
  {
    data::CircleVertex res;
    res.center = pt;

    std::vector<float> radius_v;
    radius_v.reserve( (size_t)( (_cvtx_max_radius - _cvtx_min_radius) / _cvtx_radius_step ) );
    for(float radius = _cvtx_min_radius; radius <= _cvtx_max_radius; radius += _cvtx_radius_step)
      radius_v.push_back(radius);
    
    auto const temp_xs_vv = QPointArrayOnCircleArray(img,pt,radius_v);

    for(size_t r_idx = 0; r_idx < radius_v.size(); ++r_idx) {

      auto const& radius    = radius_v[r_idx];
      auto const& temp_xs_v = temp_xs_vv[r_idx];

      if(temp_xs_v.empty()) continue;
      
      std::vector<data::PointPCA> xs_v;
      std::vector<float> dtheta_v;
      xs_v.reserve(temp_xs_v.size());
      dtheta_v.reserve(temp_xs_v.size());

      // Estimate a local PCA and center-to-xs line's angle diff
      for(size_t xs_idx=0; xs_idx < temp_xs_v.size(); ++xs_idx) {
	auto const& xs_pt = temp_xs_v[xs_idx];
	// Line that connects center to xs
	try{
	  auto local_pca   = SquarePCA(img,xs_pt,_pca_box_size,_pca_box_size);
	  auto center_line = geo2d::Line<float>(xs_pt, xs_pt - pt);
	  // Alternative (and probably better/faster): compute y spread in polar coordinate
	  LAROCV_DEBUG() << "Radius " << radius << " Line ID " << xs_idx << " found xs " << xs_pt
			 << " dtheta " << fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)) << std::endl;
	  xs_v.push_back(data::PointPCA(xs_pt,local_pca));
	  dtheta_v.push_back(fabs(geo2d::angle(center_line) - geo2d::angle(local_pca)));
	}catch(const larbys& err){
	  continue;
	}
      }

      // if this is the 1st loop, set the result
      if(res.xs_v.empty()) {
	res.radius   = radius;
	res.xs_v     = xs_v;
	res.dtheta_v = dtheta_v;
	continue;
      }

      // if this (new) radius has more crossing point, that means we started to
      // cross something else other than particle trajectory from the circle's center
      // then we break
      if(res.xs_v.size() != xs_v.size()) {
	LAROCV_DEBUG() << "Breaking @ radius = " << radius << " (not included) since # crossing point increased!" << std::endl;
	break;
      }

      // else we prefer the larger radius to have a better direction estimate for trajectories
      res.radius   = radius;
      res.xs_v     = xs_v;
      res.dtheta_v = dtheta_v;

    }


    for(size_t xs_idx=0; xs_idx<res.xs_v.size(); ++xs_idx) {
      auto const& xs_pt     = res.xs_v[xs_idx].pt;
      auto const& dtheta    = res.dtheta_v[xs_idx];
      auto center_line      = geo2d::Line<float>(xs_pt, xs_pt - res.center);
      LAROCV_INFO() << "Radius " << res.radius << " Line ID " << xs_idx << " found xs " << xs_pt
		    << " dtheta " << dtheta << std::endl;
    }

    return res;
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
    auto const inner_xs_pts = QPointOnCircle(img,geo2d::Circle<float>(pt,_radius  ));
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
	  init_vtx.xs_v.push_back(data::PointPCA(temp2_xs_v[xs_idx],temp2_pca_v[xs_idx]));
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

      // Get a step-veto regions
      //auto veto_ctor_v = VertexVetoRegion(img);
      auto const& veto_ctor_v = _veto_ctor_vv[plane];
      geo2d::Vector<int> int_pt;
      while(bbox.contains(step_pt)) {

	int_pt.x = (size_t)(step_pt.x+0.5);
	int_pt.y = (size_t)(step_pt.y+0.5);
	
	// Check if this step is within a veto region
	bool in_veto=false;
	for(auto const& veto_ctor : veto_ctor_v) {
	  if(cv::pointPolygonTest(veto_ctor,step_pt,false)<0)
	    continue;
	  in_veto = true;
	  break;
	}
	if(in_veto) {
	  step_pt += dir;
	  continue;
	}

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
	//auto circle = this->RadialScan(img,trial_pt);
	if(circle.xs_v.size() < 2) {
	  step_pt+= dir;
	  continue;
	}
	// Check if xs points are lined up "too straight"
	if(circle.xs_v.size() == 2) {
	  auto center_line0 = geo2d::Line<float>(circle.xs_v[0].pt, circle.xs_v[0].pt - circle.center);
	  auto center_line1 = geo2d::Line<float>(circle.xs_v[1].pt, circle.xs_v[1].pt - circle.center);
	  auto dtheta = fabs(geo2d::angle(center_line0) - geo2d::angle(center_line1));
	  if(dtheta < _straight_line_angle_cut){
	    step_pt += dir;
	    continue;
	  }
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

  double Refine2DVertex::CircleWeight(const larocv::data::CircleVertex& cvtx)
  {
    double dtheta_sum = cvtx.sum_dtheta();
    // check angular resolution
    double dtheta_sigma = 1./cvtx.radius * 180 / M_PI;
    dtheta_sigma = sqrt(pow(dtheta_sigma,2)*cvtx.xs_v.size());
    if(dtheta_sum>dtheta_sigma) return dtheta_sum;

    if(cvtx.xs_v.size()<2) return dtheta_sigma;

    // if dtheta better than resolution, then compute weight differently
    std::set<double> theta_loc_s;
    geo2d::Vector<float> rel_pt;
    for(auto const& xs : cvtx.xs_v) {
      rel_pt.x = xs.pt.x - cvtx.center.x;
      rel_pt.y = xs.pt.y - cvtx.center.y;
      double theta_loc=0;
      if(xs.pt.x==0.) {
	if(xs.pt.y>0) theta_loc = 90.;
	else theta_loc = 270.;
      }else{
	theta_loc = acos(std::fabs(rel_pt.x)/cvtx.radius) * 180/M_PI;
	if(rel_pt.x<0 && rel_pt.y> 0) theta_loc = 180 - theta_loc;
	if(rel_pt.x<0 && rel_pt.y<=0) theta_loc += 180;
	if(rel_pt.x>0 && rel_pt.y<=0) theta_loc = 360 - theta_loc;
      }
      //std::cout<<"("<<xs.pt.x<<","<<xs.pt.y<<") = ("<<rel_pt.x<<","<<rel_pt.y<<") ... @ " << theta_loc<<std::endl;
      theta_loc_s.insert(theta_loc);
    }

    std::vector<double> theta_loc_v;
    for(auto const& theta_loc : theta_loc_s)
      theta_loc_v.push_back(theta_loc);

    double weight=0;
    for(size_t i=0; (i+1)<theta_loc_v.size(); ++i) {
      double dtheta = theta_loc_v[i+1] - theta_loc_v[i];
      weight += std::fabs(cos(M_PI * dtheta/180.));
    }
    weight /= (double)(theta_loc_v.size()-1);
    weight *= dtheta_sigma;
    //std::cout<<cvtx.radius<<" ... "<<dtheta_sigma << " ... " << cvtx.xs_v.size() << " ... " << theta_loc_s.size() << " ... " << theta_loc_v.size() << " ... " << weight << std::endl;
    
    return weight;
  }

  void Refine2DVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
				 const ::cv::Mat& img,
				 larocv::ImageMeta& meta,
				 larocv::ROI& roi)
  {
    auto& data = AlgoData<data::Refine2DVertexData>();

    _veto_ctor_vv.resize(meta.plane()+1);

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

    // logic to avoid too-close neighbor search
    std::vector<geo2d::Circle<float> > used_circle_v;

    if(_defect_algo_id != kINVALID_ID) {
      auto const& defect_pts = AlgoData<data::DefectClusterData>(_defect_algo_id);
      for(auto const& compound : defect_pts._raw_cluster_vv[meta.plane()].get_cluster()) {
	for(auto const& defect_pt : compound.get_defects()) {
	  const auto pt = defect_pt._pt_defect;
	  // check if this point is close to what is scanned before, if so then skip
	  // "close" is defined by checking if this point is included within the radius of previous circle
	  bool used = false;
	  double dist = 0;
	  for(auto const& used_circle : used_circle_v) {
	    if(geo2d::contains(used_circle,pt,dist)) {
	      used = true;
	      break;
	    }
	  }
	  if(used) continue;
	  LAROCV_DEBUG() << "Scanning Defect point: " << pt << std::endl;
	  circle.center = pt;
	  circle.radius = _radius;
	  cv::Mat src_img;
	  if(_clean_image) {
	    _veto_ctor_vv[meta.plane()] = VertexVetoRegion(img);
	    src_img = CleanImage(img,_veto_ctor_vv[meta.plane()]);
	  }
	  else src_img = img;
	  found = PlaneScan(src_img,meta.plane(),circle,pt_err) || found;
	  circle.radius = 1;
	  used_circle_v.push_back(circle);
	}
      }
    }

    if(_pca_algo_id != kINVALID_ID) {
      auto const& pca_pts    = AlgoData<data::PCACandidatesData>(_pca_algo_id);    
      for(auto const& pt : pca_pts.points(meta.plane())) {
	// check if this point is close to what is scanned before, if so then skip
	// "close" is defined by checking if this point is included within the radius of previous circle
	bool used = false;
	double dist = 0;
	for(auto const& used_circle : used_circle_v) {
	  if(geo2d::contains(used_circle,pt,dist)) {
	    used = true;
	    break;
	  }
	}
	if(used) continue;
	LAROCV_DEBUG() << "Scanning PCACandidate point: " << pt << std::endl;
	circle.center = pt;
	circle.radius = _radius;
	cv::Mat src_img;
	if(_clean_image) {
	  _veto_ctor_vv[meta.plane()] = VertexVetoRegion(img);
	  src_img = CleanImage(img,_veto_ctor_vv[meta.plane()]);
	}
	else src_img = img;
	found = PlaneScan(src_img,meta.plane(),circle,pt_err) || found;
	circle.radius = 1;
	used_circle_v.push_back(circle);
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

  void Refine2DVertex::XPlaneTimeProposal()
  {
    //auto& data = AlgoData<larocv::Refine2DVertexData>();
    // Find local minimum for >2 plane score array
    auto const& score0_v    = _time_binned_score0_v;
    auto& mean_score0_v     = _time_binned_score0_mean_v;
    auto& minidx_score0_v   = _time_binned_score0_minidx_v;
    auto& minrange_score0_v = _time_binned_score0_minrange_v;
    
    mean_score0_v = RollingMean(score0_v,3,3,-1);
    ExtremePoints(mean_score0_v,3,3,true,true,minidx_score0_v,minrange_score0_v,-1);

    auto const& score1_v    = _time_binned_score1_v;
    auto& mean_score1_v     = _time_binned_score1_mean_v;
    auto& minidx_score1_v   = _time_binned_score1_minidx_v;
    auto& minrange_score1_v = _time_binned_score1_minrange_v;
    mean_score1_v = RollingMean(score1_v,3,3,-1);
    ExtremePoints(mean_score1_v,3,3,true,true,minidx_score1_v,minrange_score1_v,-1);

    // Now work on excluding neighbors for local minimum tick estimation
    auto& minidx_v   = _time_binned_minidx_v;
    auto& minrange_v = _time_binned_minrange_v;
    std::vector<float> minscore_v;
    std::vector<bool>  threeplane_v;
    
    for(size_t i=0; i<minidx_score1_v.size(); ++i) {

      auto const& candidate_idx   = minidx_score1_v[i];
      auto const& candidate_range = minrange_score1_v[i];
      auto const& candidate_score = mean_score1_v[candidate_idx];
      
      // Check if this candidate is within the exclusion radius of the previously registered index
      // If so, pick the "smaller score" one
      bool overlap=false;
      for(size_t i=0; i<minidx_v.size(); ++i) {
	auto const& prev_idx = minidx_v[i];
	int dist = std::abs((int)(candidate_idx) - (int)(prev_idx));
	if(dist < _time_exclusion_radius) {
	  overlap = true;
	  if(minscore_v[i] > candidate_score) {
	    //update
	    minidx_v[i]   = candidate_idx;
	    minrange_v[i] = candidate_range;
	    minscore_v[i] = candidate_score;
	  }
	}
      }
      if(!overlap) {
	minidx_v.push_back(candidate_idx);
	minrange_v.push_back(candidate_range);
	minscore_v.push_back(candidate_score);
	threeplane_v.push_back(true);
      }
    }

    for(size_t i=0; i<minidx_v.size(); ++i)
      LAROCV_INFO() << "Found valid 3-plane binned tick @ " << minidx_v[i] << " score " << minscore_v[i] << std::endl;

    for(size_t i=0; i<minidx_score0_v.size(); ++i) {

      auto const& candidate_idx   = minidx_score0_v[i];
      auto const& candidate_range = minrange_score0_v[i];
      auto const& candidate_score = mean_score0_v[candidate_idx];
      
      // Check if this candidate is within the exclusion radius of the previously registered index
      // If so, pick the "smaller score" one
      bool overlap=false;
      for(size_t i=0; i<minidx_v.size(); ++i) {
	auto const& prev_idx = minidx_v[i];
	int dist = std::abs((int)(candidate_idx) - (int)(prev_idx));
	if(dist < _time_exclusion_radius) {
	  overlap = true;
	  if(!threeplane_v[i] && minscore_v[i] > candidate_score) {
	    //update
	    minidx_v[i]   = candidate_idx;
	    minrange_v[i] = candidate_range;
	    minscore_v[i] = candidate_score;
	  }
	}
      }
      if(!overlap) {
	minidx_v.push_back(candidate_idx);
	minrange_v.push_back(candidate_range);
	minscore_v.push_back(candidate_score);
	threeplane_v.push_back(false);
      }
    }

    for(size_t i=0; i<minidx_v.size(); ++i) {
      if(!threeplane_v[i]) {
	LAROCV_INFO() << "Found valid 2-plane binned tick @ " << minidx_v[i] << " score " << minscore_v[i] << std::endl;
      }
    }
  }

  void Refine2DVertex::TimeVertex3D(const std::vector<const cv::Mat>& img_v)
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
	//auto const  weight0 = circle0.sum_dtheta();
	auto const  weight0 = CircleWeight(circle0);
	for(auto const& circle1_idx : seed_circle_idx_v[seed1_plane]) {
	  auto const& circle1 = seed1_data._circle_scan_v[circle1_idx];
	  //auto const  weight1 = circle1.sum_dtheta();
	  auto const  weight1 = CircleWeight(circle1);
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

	  float ave_dtheta = weight0 + weight1;
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
	    float  check_weight  = 0;
	    for(auto const& circle2_idx : seed_circle_idx_v[check_plane]) {
	      auto const& circle2 = check_data._circle_scan_v[circle2_idx];
	      //auto const  weight2 = circle2.sum_dtheta();
	      auto const  weight2 = CircleWeight(circle2);
	      float dist = std::fabs(_origin_v[check_plane].y + circle2.center.y * _wire_comp_factor_v[check_plane] - approx_wire2 );
	      if(dist < dist_min) {
		dist_min = dist;
		closest_circle2_idx = circle2_idx;
		check_weight = weight2;
	      }
	    }
	    if(dist_min > _xplane_wire_resolution) closest_circle2_idx = kINVALID_SIZE;
	    ave_dtheta += check_weight;
	  }
	  if(closest_circle2_idx != kINVALID_SIZE) {
	    ave_dtheta /= 3.;
	    std::vector<size_t> mapval(3);
	    mapval[0] = circle0_idx; mapval[1] = circle1_idx; mapval[2] = 3;
	    scoremap_3plane.emplace(ave_dtheta,mapval);
	    LAROCV_DEBUG() << "Found a 3-plane vertex candiate @ (y,z) = (" << y << "," << z << ") ... "
			   << "plane " << seed0_plane << " @ " << circle0.center << " ... "
			   << "plane " << seed1_plane << " @ " << circle1.center << " ... "
			   << "dtheta = " << ave_dtheta << std::endl;
	  }
	  else {
	    ave_dtheta /= 2.;
	    std::vector<size_t> mapval(3);
	    mapval[0] = circle0_idx; mapval[1] = circle1_idx; mapval[2] = 2;
	    scoremap_2plane.emplace(ave_dtheta,mapval);
	    LAROCV_DEBUG() << "Found a 2-plane vertex candiate @ (y,z) = (" << y << "," << z << ") ... "
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
	LAROCV_INFO() << "Claiming time-vertex candiate @ (y,z) = (" << y << "," << z << ") ... "
		      << "plane " << seed0_plane << " @ " << circle0.center << " # crossing = " << circle0.xs_v.size() << " ... "
		      << "plane " << seed1_plane << " @ " << circle1.center << " # crossing = " << circle1.xs_v.size() << std::endl;
	
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
	    // If no candidate found among those searched, make a new search aattempt here
	    geo2d::Vector<float> guess_pt;
	    guess_pt.x = approx_x;
	    guess_pt.y = approx_y;
	    guess_pt = MeanPixel(img_v[plane],guess_pt,
				 (size_t)(_xplane_tick_resolution+0.5),
				 (size_t)(_xplane_wire_resolution+0.5));
	    if(guess_pt.x<0) {
	      guess_pt.x = approx_x;
	      guess_pt.y = approx_y;
	    }
	    //auto guess_circle = TwoPointInspection(img_v[plane],guess_pt);
	    auto guess_circle = RadialScan(img_v[plane],guess_pt);
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
	  
	  LAROCV_INFO() << "Non-seed plane " << plane << " @ " << circle_vtx_v[plane].center
			<< " # crossing = " << circle_vtx_v[plane].xs_v.size() << std::endl;
	}
	data.emplace_back(0,std::move(vtx3d),std::move(circle_vtx_v));
      }
    }
  }

  void Refine2DVertex::XPlaneWireScan(const std::vector<const cv::Mat>& img_v)
  {
    // Compute 1D projection score array per plane
    auto& data = AlgoData<data::Refine2DVertexData>();
    for(size_t plane=0; plane<img_v.size(); ++plane)
      _wire_binned_score_vv.at(plane).clear();

    for(size_t plane_id=0; plane_id<img_v.size(); ++plane_id) {
      auto& wire_binned_score_v = _wire_binned_score_vv.at(plane_id);
      auto& xplane_wire_min = _xplane_wire_min_v.at(plane_id);
      auto& xplane_wire_max = _xplane_wire_max_v.at(plane_id);

      // Figure out min/max
      auto& plane_data = data.get_plane_data_writeable(plane_id);
      if(!plane_data._valid_plane) continue;

      xplane_wire_min = 1e9;
      xplane_wire_max = 0;
      auto const& circle_scan_v  = plane_data._circle_scan_v;
      for(auto const& circle : circle_scan_v) {
	if(circle.center.y < xplane_wire_min) xplane_wire_min = circle.center.y;
	if(circle.center.y > xplane_wire_max) xplane_wire_max = circle.center.y;
      }
      //size_t num_wires = (size_t)( (xplane_wire_max - xplane_wire_min + 0.5) / _xplane_wire_resolution ) + 1;
      size_t num_wires = (size_t)( (xplane_wire_max - xplane_wire_min + 0.5) ) + 1;
      wire_binned_score_v.resize(num_wires,-1);
      for(auto const& circle : circle_scan_v) {
	size_t idx = (size_t)((circle.center.y - xplane_wire_min) + 0.5);
	auto const dtheta = circle.sum_dtheta();
	if(wire_binned_score_v[idx]<0 || wire_binned_score_v[idx] > dtheta)
	  wire_binned_score_v[idx] = dtheta;
      }
    }

    return;
  }

  void Refine2DVertex::XPlaneWireProposal() 
  {
    for(size_t plane_id=0; plane_id<_wire_binned_score_vv.size(); ++plane_id) {
      auto const& score_v    = _wire_binned_score_vv.at(plane_id);
      auto& mean_score_v     = _wire_binned_score_mean_vv.at(plane_id);
      auto& minidx_score_v   = _wire_binned_score_minidx_vv.at(plane_id);
      auto& minrange_score_v = _wire_binned_score_minrange_vv.at(plane_id);
      mean_score_v = RollingMean(score_v,3,3,-1);
      ExtremePoints(mean_score_v,3,3,true,false,minidx_score_v,minrange_score_v,-1);
    }
  }

  void Refine2DVertex::WireVertex3D(const std::vector<const cv::Mat>& img_v)
  {
    auto& data = AlgoData<data::Refine2DVertexData>();

    // List of already existing vertex (later used to avoid duplicate vertex)
    std::vector<geo2d::Vector<float> > vtx_yz_v;
    /*
    for(auto const& vtx3d : data.get_vertex())
      vtx_yz_v.push_back(geo2d::Vector<float>(vtx3d.y,vtx3d.z));
    */
    
    // Find possible combination of wires across planes
    // Loop over combination of 2 planes and find possible vertex w/ 2 or 3-plane match distinction
    std::map<float,std::vector<std::pair<size_t,float> > > cand_vtx_m;
    for(size_t plane_idx1=0; plane_idx1<_seed_plane_v.size(); ++plane_idx1) {
      // Retrieve plane A's ID, circle vertex array, and local minima index & score
      auto const& seed_plane1 = _seed_plane_v[plane_idx1];
      auto const& seed_data1  = data.get_plane_data(seed_plane1);
      if(!seed_data1._valid_plane) continue;
      auto const& circle1_v   = seed_data1._circle_scan_v;      
      auto const& wire_binned_score1_v  = _wire_binned_score_mean_vv.at(seed_plane1);
      auto const& wire_binned_minidx1_v = _wire_binned_score_minidx_vv.at(seed_plane1);
      auto const& img1 = img_v[seed_plane1];
      
      // loop over paired plane B(s)
      for(size_t plane_idx2=plane_idx1+1; plane_idx2<_seed_plane_v.size(); ++plane_idx2) {
	// Retrieve plane B's ID, circle vertex array, and local minima index & score
	auto const& seed_plane2 = _seed_plane_v[plane_idx2];
	auto const& seed_data2  = data.get_plane_data(seed_plane2);
	if(!seed_data2._valid_plane) continue;
	auto const& circle2_v   = seed_data2._circle_scan_v;
	auto const& wire_binned_score2_v  = _wire_binned_score_mean_vv.at(seed_plane2);
	auto const& wire_binned_minidx2_v = _wire_binned_score_minidx_vv.at(seed_plane2);
	auto const& img2 = img_v[seed_plane2];

	LAROCV_DEBUG() << "Scanning seed plane A=" << seed_plane1 << " B=" << seed_plane2 << std::endl;

	// Now loop over un-ordered local minima for plane A
	for(size_t minidx1_idx=0; minidx1_idx<wire_binned_minidx1_v.size(); ++minidx1_idx) {
	  auto const& score_idx1 = wire_binned_minidx1_v[minidx1_idx];
	  //auto const& score1  = wire_binned_score1_v[score_idx1];
	  float  min_wire1    = this->WireBinMin(seed_plane1);
	  float  wire1        = min_wire1 + score_idx1;
	  size_t raw_wire1    = (size_t)(wire1 * _wire_comp_factor_v.at(seed_plane1) + _origin_v[seed_plane1].y + 0.5);
	  auto const wire2_range = larocv::OverlapWireRange(raw_wire1, seed_plane1, seed_plane2);

	  LAROCV_DEBUG() << "Inspecting plane A=" << seed_plane1
			 << " minimum @ " << wire1 << " (wire=" << raw_wire1
			 << " ... range in B=" << seed_plane2
			 << " is " << wire2_range.first << " => " << wire2_range.second << ")"
			 << std::endl;

	  for(size_t minidx2_idx=0; minidx2_idx<wire_binned_minidx2_v.size(); ++minidx2_idx) {
	    auto const& score_idx2 = wire_binned_minidx2_v[minidx2_idx];
	    //auto const& score2 = wire_binned_score2_v[score_idx2];
	    float  min_wire2   = this->WireBinMin(seed_plane2);
	    float  wire2       = min_wire2 + score_idx2;
	    size_t raw_wire2   = (size_t)(wire2 * _wire_comp_factor_v.at(seed_plane2) + _origin_v[seed_plane2].y + 0.5);
	    
	    // check if two wires could overlap
	    if( raw_wire2 < (wire2_range.first  - _xplane_wire_resolution) ||
	        raw_wire2 > (wire2_range.second + _xplane_wire_resolution) ) {
	      LAROCV_DEBUG() << "Skipping B=" << seed_plane2 << " minimum @ " << wire2
			     << "(wire=" << raw_wire2 << ")" << std::endl;
	      continue;
	    }
	    // Get the actual range limit
	    // within some resolution and combination of these two wires,
	    // find all possible combinations that also agree within the
	    // configured tick resolution
	    // 0) search a list of circles near wire1 and wire2
	    // 1) search a possible combination between a pair of Circles (within tick/wire resolution)
	    // 2) see if there's a compatible plane

	    std::multimap<float,size_t> circle1_idx_m;
	    std::multimap<float,size_t> circle2_idx_m;
	    for(size_t circle1_idx=0; circle1_idx<circle1_v.size(); ++circle1_idx) {
	      auto const& circle1 = circle1_v[circle1_idx];
	      if(std::fabs(circle1.center.y - wire1) > _xplane_wire_resolution) continue;
	      //circle1_idx_m.insert(std::make_pair(circle1.sum_dtheta(),circle1_idx));
	      circle1_idx_m.insert(std::make_pair(CircleWeight(circle1),circle1_idx));
	    }
	    for(size_t circle2_idx=0; circle2_idx<circle2_v.size(); ++circle2_idx) {
	      auto const& circle2 = circle2_v[circle2_idx];
	      if(std::fabs(circle2.center.y - wire2) > _xplane_wire_resolution) continue;
	      //circle2_idx_m.insert(std::make_pair(circle2.sum_dtheta(),circle2_idx));
	      circle2_idx_m.insert(std::make_pair(CircleWeight(circle2),circle2_idx));
	    }

	    LAROCV_INFO() << "Wire vertex candidate: Plane A=" << seed_plane1
			  << " minimum @ " << wire1 << " (wire=" << raw_wire1 << ") # circles " << circle1_idx_m.size()
			  << std::endl;
	    LAROCV_INFO() << "Wire vertex candidate: Plane B=" << seed_plane2
			  << " minimum @ " << wire2 << " (wire=" << raw_wire2 << ") # circles " << circle2_idx_m.size()
			  << std::endl;
	    
	    // Loop over to make all possible pairs
	    std::set<size_t> seed1_used_idx_s;
	    std::set<size_t> seed2_used_idx_s;
	    for(auto const& score_idx_pair1 : circle1_idx_m) {
	      auto const& score1      = score_idx_pair1.first;
	      auto const& circle1_idx = score_idx_pair1.second;
	      auto const& circle1     = circle1_v[circle1_idx];
	      for(auto const& score_idx_pair2 : circle2_idx_m) {
		auto const& score2      = score_idx_pair2.first;
		auto const& circle2_idx = score_idx_pair2.second;
		auto const& circle2     = circle2_v[circle2_idx];

		float tick_dist = std::fabs( (circle1.center.x - _tick_offset_v[seed_plane1] / _time_comp_factor_v[seed_plane1])
					     -
					     (circle2.center.x - _tick_offset_v[seed_plane2] / _time_comp_factor_v[seed_plane2])
					     );
		if(tick_dist > (float)(_xplane_tick_resolution + 1.0)) {
		  LAROCV_DEBUG() << "Pair ... " << circle1.center << " on A ... " << circle2.center << " on B ... "
				 << "Skipping: too big tick range difference" << std::endl;
		  continue;
		}
		
		if(seed1_used_idx_s.find(circle1_idx) != seed1_used_idx_s.end()) {
		  LAROCV_DEBUG() << "Pair ... " << circle1.center << " on A ... " << circle2.center << " on B ... "
				 << "Skipping: already claimed circle on plane A" << std::endl;
		  continue;
		}
		if(seed2_used_idx_s.find(circle2_idx) != seed2_used_idx_s.end()) {
		  LAROCV_DEBUG() << "Pair ... " << circle1.center << " on A ... " << circle2.center << " on B ... "
				 << "Skipping: already claimed circle on plane B" << std::endl;
		  continue;
		}
		// At this point we claim a solid candidate for >= 2 planes
		// Construct solid wire range respecting circle1
		size_t circle1_wire    = (size_t)(circle1.center.y * _wire_comp_factor_v.at(seed_plane1) + _origin_v[seed_plane1].y + 0.5);
		size_t circle2_wire    = (size_t)(circle2.center.y * _wire_comp_factor_v.at(seed_plane2) + _origin_v[seed_plane2].y + 0.5);
		auto const wire2_range = larocv::OverlapWireRange(circle1_wire, seed_plane1, seed_plane2);
		if(circle2_wire < wire2_range.first ) circle2_wire = wire2_range.first;
		if(circle2_wire > wire2_range.second) circle2_wire = wire2_range.second;
		// Get 3D vertex
		double y,z;
		larocv::IntersectionPoint(circle1_wire,seed_plane1,circle2_wire,seed_plane2,y,z);
		// Check if this vertex can be taken as a unique one w/o neighbors
		bool neighbor=false;
		geo2d::Vector<float> yz_pt(y,z);
		for(auto const& pt : vtx_yz_v) {
		  if(geo2d::dist(pt,yz_pt) < _vtx_3d_resolution) {
		    neighbor = true;
		    break;
		  }
		}
		if(neighbor) {

		  LAROCV_INFO() << "Ignoring a too-close-neighbor pair: plane " << seed_plane1
				<< " @ " << circle1.center << " dtheta=" << circle1.sum_dtheta()
				<< " ... plane " << seed_plane2
				<< " @ " << circle2.center << " dtheta=" << circle2.sum_dtheta()
				<< std::endl;
		  continue;
		}

		// Concrete vertex found
		LAROCV_INFO() << "Claiming wire-vertex candiate @ (y,z) = (" << y << "," << z << ") ... "
			      << "seed plane " << seed_plane1 << " @ " << circle1.center << " # crossing = " << circle1.xs_v.size() << " ... "
			      << "seed plane " << seed_plane2 << " @ " << circle2.center << " # crossing = " << circle2.xs_v.size() << std::endl;

		vtx_yz_v.emplace_back(yz_pt);
		seed1_used_idx_s.insert(circle1_idx);
		seed2_used_idx_s.insert(circle2_idx);

		// Construct & fill 3D vertex container
		data::Vertex3D vtx3d;
		vtx3d.y = y;
		vtx3d.z = z;
		vtx3d.x = larocv::TriggerTick2Cm(circle1.center.x * _time_comp_factor_v[seed_plane1] +
						 _origin_v[seed_plane1].x - _trigger_tick);
		vtx3d.num_planes = 2;
		vtx3d.vtx2d_v.resize(img_v.size());
		vtx3d.vtx2d_v[seed_plane1].pt    = circle1.center;
		vtx3d.vtx2d_v[seed_plane1].score = circle1.sum_dtheta();
		vtx3d.vtx2d_v[seed_plane2].pt    = circle2.center;
		vtx3d.vtx2d_v[seed_plane2].score = circle2.sum_dtheta();

		std::vector<larocv::data::CircleVertex> circle_vtx_v(img_v.size());
		circle_vtx_v[seed_plane1] = circle1;
		circle_vtx_v[seed_plane2] = circle2;

		float approx_x = (circle1.center.x + circle2.center.x)/2.;
		// record other planes'
		for(size_t plane=0; plane<img_v.size(); ++plane) {
		  size_t closest_idx = kINVALID_SIZE;
		  float  approx_wire = larocv::WireCoordinate(y,z,plane);
		  float  approx_y = (approx_wire - _origin_v[plane].y) / _wire_comp_factor_v[plane];
		  float  dist_min = 1e9;
		  auto const& plane_data = data.get_plane_data(plane);
		  auto const& circle_v   = plane_data._circle_scan_v;
		  float  closest_dtheta = -1;
		  // from here
		  for(size_t circle_idx=0; circle_idx < circle_v.size(); ++circle_idx) {
		    auto const& circle = circle_v[circle_idx];
		    auto const  dtheta = circle.sum_dtheta();
		    if(fabs(approx_y - circle.center.y) > _xplane_wire_resolution) continue;
		    if(fabs(approx_x - circle.center.x) > _xplane_tick_resolution) continue;
		    
		    float dist = std::fabs(approx_y - circle.center.y);
		    if(dist < dist_min) {
		      dist_min = dist;
		      closest_idx = circle_idx;
		      closest_dtheta = dtheta;
		    }
		  }

		  // Fill candidate 2D CircleVertex for non-seed planes
		  if(closest_idx != kINVALID_SIZE) {
		    circle_vtx_v[plane] = circle_v[closest_idx];
		    vtx3d.vtx2d_v[plane].score = circle_vtx_v[plane].sum_dtheta();
		    vtx3d.num_planes += 1;
		  }
		  else {
		    // If no candidate found among those searched, make a new search aattempt here
		    geo2d::Vector<float> guess_pt;
		    guess_pt.x = approx_x;
		    guess_pt.y = approx_y;
		    guess_pt = MeanPixel(img_v[plane],guess_pt,
					 (size_t)(_xplane_tick_resolution+0.5),
					 (size_t)(_xplane_wire_resolution+0.5));
		    if(guess_pt.x<0) {
		      guess_pt.x = approx_x;
		      guess_pt.y = approx_y;
		    }
		    //auto guess_circle = TwoPointInspection(img_v[plane],guess_pt);
		    auto guess_circle = RadialScan(img_v[plane],guess_pt);
		    if(guess_circle.xs_v.empty()) {
		      guess_circle.center = guess_pt;
		      guess_circle.radius = _radius;
		    }
		    circle_vtx_v[plane] = guess_circle;
		  }

		  LAROCV_INFO() << "Claiming non-seed 2D vertex for plane " << plane
				<< " @ " << circle_vtx_v[plane].center
				<< " # crossing = " << circle_vtx_v[plane].xs_v.size() << std::endl;
		  
		  vtx3d.vtx2d_v[plane].pt.x  = approx_x;
		  vtx3d.vtx2d_v[plane].pt.y  = approx_y;

		  if(!vtx3d.vtx2d_v[plane].score) vtx3d.vtx2d_v[plane].score = -1;
		  
		  LAROCV_INFO() << "Non-seed plane " << plane << " @ " << circle_vtx_v[plane].center
				<< " # crossing = " << circle_vtx_v[plane].xs_v.size() << std::endl;
		} // finish looping over non-seed planes

		data.emplace_back(1,std::move(vtx3d),std::move(circle_vtx_v));
	      }
	    }
	    LAROCV_INFO() << "Found pairs for this combo: " << seed1_used_idx_s.size() << std::endl;
	  }
	}
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
    _time_binned_score0_minrange_v.clear();
    _time_binned_score1_v.clear();
    _time_binned_score1_mean_v.clear();
    _time_binned_score1_minidx_v.clear();
    _time_binned_score1_minrange_v.clear();
    _time_binned_minidx_v.clear();
    _time_binned_minrange_v.clear();

    XPlaneTimeScan(img_v);
    XPlaneTimeProposal();
    TimeVertex3D(img_v);
    
    _xplane_wire_min_v.clear();
    _xplane_wire_max_v.clear();
    _wire_binned_score_vv.clear();
    _wire_binned_score_mean_vv.clear();
    _wire_binned_score_minidx_vv.clear();
    _wire_binned_score_minrange_vv.clear();

    _xplane_wire_min_v.resize(img_v.size());
    _xplane_wire_max_v.resize(img_v.size());
    _wire_binned_score_vv.resize(img_v.size());
    _wire_binned_score_mean_vv.resize(img_v.size());
    _wire_binned_score_minidx_vv.resize(img_v.size());
    _wire_binned_score_minrange_vv.resize(img_v.size());

    XPlaneWireScan(img_v);
    XPlaneWireProposal();
    WireVertex3D(img_v);
    return true;
  }

}
#endif
