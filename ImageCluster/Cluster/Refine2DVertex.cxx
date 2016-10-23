#ifndef __REFINE2DVERTEX_CXX__
#define __REFINE2DVERTEX_CXX__

#include "Refine2DVertex.h"
#include "CircleVertexData.h"
#include "Core/HalfLine.h"
#include "Core/Line.h"
#include "Core/BoundingBox.h"
#include <map>
namespace larocv{

  /// Global larocv::Refine2DVertexFactory to register ClusterAlgoFactory
  static Refine2DVertexFactory __global_Refine2DVertexFactory__;

  void Refine2DVertex::_Configure_(const ::fcllite::PSet &pset)
  {
    auto const circle_vtx_algo_name = pset.get<std::string>("CircleVertexAlgo");
    _circle_vertex_algo_id = this->ID(circle_vtx_algo_name);
    _radius = 10;
    _pi_threshold = 10;
    _pca_box_size = 2;
    _global_bound_size = 25;
  }

  geo2d::Line<float> Refine2DVertex::SquarePCA(const ::cv::Mat& img,
					       geo2d::Vector<float> pt,
					       const size_t side)
  {
					       
    auto small_img = ::cv::Mat(img,::cv::Rect(pt.x-side, pt.y-side, 2*side+1, 2*side+1));
    ::cv::Mat thresh_small_img;
    ::cv::threshold(small_img,thresh_small_img,_pi_threshold,1,CV_THRESH_BINARY);
    geo2d::VectorArray<int> points;
    findNonZero(thresh_small_img, points);
    
    cv::Mat mat_points(points.size(), 2, CV_32FC1);
    for (unsigned i = 0; i < mat_points.rows; ++i) {
      mat_points.at<float>(i, 0) = points[i].x;
      mat_points.at<float>(i, 1) = points[i].y;
    }
    
    ::cv::PCA pcaobj(mat_points,::cv::Mat(),::cv::PCA::DATA_AS_ROW,0);
    
    //std::cout<<pt.x<<","<<pt.y<<" ... " << pcaobj.mean.at<float>(0,0) << "," << pcaobj.mean.at<float>(0,1) << std::endl;
    pt.x += pcaobj.mean.at<float>(0,0) - side;
    pt.y += pcaobj.mean.at<float>(0,1) - side;
    
    auto dir = geo2d::Vector<float>(pcaobj.eigenvectors.at<float>(0,0),
				    pcaobj.eigenvectors.at<float>(0,1));

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

  Cluster2DArray_t Refine2DVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
					     const ::cv::Mat& img,
					     larocv::ImageMeta& meta,
					     larocv::ROI& roi)
  {
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
    std::cout << bbox << std::endl;
    // Now we loop over per-xs-on-init-circle, and find a candidate vertex per xs point
    std::multimap< double, geo2d::Vector<float> > candidate_vtx_m;
    for(size_t xs_idx=0; xs_idx < xs_v.size(); ++xs_idx){

      auto const& xs  = xs_v [xs_idx];
      auto const& pca = pca_v[xs_idx];

      // Get a unit vector to traverse, starting from X-min edge point
      geo2d::Vector<float> start(top_left.x, pca.y(top_left.x));
      geo2d::Vector<float> end(bottom_right.x, pca.y(bottom_right.x));
      auto const dir = ::geo2d::dir(start,end);
      std::cout<<"start: " << start << " ... " << "end: " << end << " ... " << "dir " << dir << std::endl;
      
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
      std::cout << step_pt << ", " << q << std::endl;
      
      while(bbox.contains(step_pt)) {

	// Check if this point has any charge. if not continue
	col = (size_t)(step_pt.x);
	row = (size_t)(step_pt.y);
	q = (float)(img.at<unsigned char>(row,col));

	std::cout << start << " ... " << step_pt << ", " << q << std::endl;

	if(q < _pi_threshold) {
	  step_pt += dir;
	  continue;
	}

	// Get xs points for this step. if < 2 continue
	auto const step_xs_pts = QPointOnCircle(img,geo2d::Circle<float>(step_pt,_radius));

	data._circle_trav_vv[meta.plane()].emplace_back(std::move(geo2d::Circle<float>(step_pt,_radius)));
	
	if(step_xs_pts.size()<2) {
	  step_pt += dir;
	  continue;
	}
	
	if(step_xs_pts.size()==2) {
	  auto center_line0 = geo2d::Line<float>(step_xs_pts[0], step_xs_pts[0] - step_pt);
	  auto center_line1 = geo2d::Line<float>(step_xs_pts[1], step_xs_pts[1] - step_pt);
	  auto dtheta = fabs(geo2d::angle(center_line0) - geo2d::angle(center_line1));
	  if(dtheta < 10) {
	    step_pt+= dir;
	    continue;
	  }
	}

	double dtheta_sum = 0;
	for(auto const& step_xs_pt : step_xs_pts) {
	  // Line that connects center to xs
	  auto center_line = geo2d::Line<float>(step_xs_pt, step_xs_pt - step_pt);
	  auto local_pca   = SquarePCA(img,step_xs_pt,_pca_box_size);
	  dtheta_sum += fabs(geo2d::angle(center_line) - geo2d::angle(local_pca));
	}

	if(dtheta_sum < min_dtheta_sum) {
	  min_dtheta_sum = dtheta_sum;
	  candidate_vtx = step_pt;
	}
	
	// Increment the step and continue or break
	step_pt += dir;	
      }

      // Fine tuning w/ neighbor pixels
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
    }

    for(auto const& score_vtx : candidate_vtx_m) {
      data._cand_score_vv[meta.plane()].push_back(score_vtx.first);
      data._cand_vtx_vv[meta.plane()].push_back(score_vtx.second);      
    }


    
    return clusters;
  }

}
#endif
