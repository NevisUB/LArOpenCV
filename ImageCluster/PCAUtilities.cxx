//by vic
#ifndef __IMAGECLUSTER_PCAUTILITIES_CXX__
#define __IMAGECLUSTER_PCAUTILITIES_CXX__

#include "PCAUtilities.h"
#include "StatUtilities.h"

#include "PCAPath.h"

namespace larcv {


  double roi_d_to_line(const std::vector<double>& line,int lx,int ly) {

    //real time collision detection page 128
    //closest point on line
    auto& ax = line[0];
    auto& ay = line[1];
    auto& bx = line[2];
    auto& by = line[3];
	
    auto abx = bx-ax;
    auto aby = by-ay;
	
    auto t = ( (lx-ax)*abx + (ly-ay)*aby ) / (abx*abx + aby*aby);
    auto dx = ax + t * abx;
    auto dy = ay + t * aby;
    auto dist = (dx-lx)*(dx-lx) + (dy-ly)*(dy-ly); //squared distance
    return std::sqrt( dist );
  }

  double roi_d_to_line(const Point2D& dir, const Point2D& pt,int lx,int ly) {

    //real time collision detection page 128
    //closest point on line
    auto& ax = pt.x;
    auto& ay = pt.y;
    auto  bx = pt.x + dir.x;
    auto  by = pt.y + dir.y;
	
    auto abx = bx-ax;
    auto aby = by-ay;
	
    auto t = ( (lx-ax)*abx + (ly-ay)*aby ) / (abx*abx + aby*aby);
    auto dx = ax + t * abx;
    auto dy = ay + t * aby;
    auto dist = (dx-lx)*(dx-lx) + (dy-ly)*(dy-ly); //squared distance
    return std::sqrt( dist );
  }

  double total_d_pca(const PCABox& box) {
    double tot = 0.0;
    
    for(const auto& pt : box.pts_)
      tot += roi_d_to_line(box.line_,pt.x + box.parent_roi_.x,pt.y + box.parent_roi_.y);

    return tot;
    
  }


  double total_cw_d_pca(const PCABox& box) {
    double tot = 0.0;

    for(unsigned i = 0; i < box.pts_.size(); ++i) {
      auto& pt = box.pts_   [i];
      auto& ch = box.charge_[i];
      
      tot += ch * roi_d_to_line(box.line_,pt.x + box.parent_roi_.x,pt.y + box.parent_roi_.y);
    }
    
    return tot;
    
  }
  
  double avg_d_pca(const PCABox& box) {
    return total_d_pca(box) / ( (double) box.pts_.size() );
  }
  
  double cw_d_pca(const PCABox& box) {
    if ( ! box.charge_.size() ) { std::cout << "Set charge first\n"; throw std::exception(); }

    double cs = static_cast<double>( box.charge_sum() );

    return total_cw_d_pca(box) / cs;
  }
  
  
  void pca_line(const Contour_t& cluster_s,
		Point2D& e_vec,
		Point2D& e_center) {
    
    ::cv::Mat ctor_pts(cluster_s.size(), 2, CV_64FC1);
    
    for (unsigned i = 0; i < ctor_pts.rows; ++i) {
      ctor_pts.at<double>(i, 0) = cluster_s[i].x;
      ctor_pts.at<double>(i, 1) = cluster_s[i].y;
    }
    
    ::cv::PCA pca_ana(ctor_pts, ::cv::Mat(), CV_PCA_DATA_AS_ROW,0);

    e_center = Point2D( pca_ana.mean.at<double>(0,0),
			pca_ana.mean.at<double>(0,1) );
    
    e_vec    = Point2D( pca_ana.eigenvectors.at<double>(0,0),
			pca_ana.eigenvectors.at<double>(0,1) );
    
    auto& ax = e_vec.x;
    auto& ay = e_vec.y;
    auto  sq = std::sqrt(ax*ax + ay*ay);
    
    ax /= sq;
    ay /= sq;

  }


  
  void pca_line(const Contour_t& cluster_s,
		const ::cv::Rect& roi, // assumes this is ROI coming in...
		std::vector<double>& line,
		Point2D& e_vec,
		Point2D& e_center) {
    
    ::cv::Mat ctor_pts(cluster_s.size(), 2, CV_64FC1);
    
    for (unsigned i = 0; i < ctor_pts.rows; ++i) {
      ctor_pts.at<double>(i, 0) = cluster_s[i].x;
      ctor_pts.at<double>(i, 1) = cluster_s[i].y;
    }
    
    ::cv::PCA pca_ana(ctor_pts, ::cv::Mat(), CV_PCA_DATA_AS_ROW,0);

    e_center = Point2D( pca_ana.mean.at<double>(0,0),
			pca_ana.mean.at<double>(0,1) );
    
    e_vec    = Point2D( pca_ana.eigenvectors.at<double>(0,0),
			pca_ana.eigenvectors.at<double>(0,1) );
    
    auto& ax = e_vec.x;
    auto& ay = e_vec.y;
    auto  sq = std::sqrt(ax*ax + ay*ay);
    
    ax /= sq;
    ay /= sq;
    
    line[0] = 0 + roi.x;
    line[1] = e_center.y + ( (0 - e_center.x) / e_vec.x ) * e_vec.y + roi.y;
    line[2] = roi.width + roi.x;
    line[3] = e_center.y + ( (roi.width - e_center.x) / e_vec.x) * e_vec.y + roi.y;
  }
  
  void pca_line(const Contour_t& cluster_s,
		const ::cv::Rect& roi,
		const ::cv::Rect& rect,
		std::vector<double>& line,
		Point2D& e_vec,
		Point2D& e_center) {
    
    ::cv::Mat ctor_pts(cluster_s.size(), 2, CV_64FC1);
    
    for (unsigned i = 0; i < ctor_pts.rows; ++i) {
      ctor_pts.at<double>(i, 0) = cluster_s[i].x;
      ctor_pts.at<double>(i, 1) = cluster_s[i].y;
    }
    
    ::cv::PCA pca_ana(ctor_pts, ::cv::Mat(), CV_PCA_DATA_AS_ROW,0);

    e_center = Point2D( pca_ana.mean.at<double>(0,0),
			pca_ana.mean.at<double>(0,1) );
    
    e_vec    = Point2D( pca_ana.eigenvectors.at<double>(0,0),
			pca_ana.eigenvectors.at<double>(0,1) );
    
    auto& ax = e_vec.x;
    auto& ay = e_vec.y;

    auto cx = e_center.x;
    auto cy = e_center.y;
    
    auto  sq = std::sqrt(ax*ax + ay*ay);
    
    ax /= sq;
    ay /= sq;
    
    line[0] = rect.x;
    line[1] = cy + ( (rect.x - cx) / e_vec.x ) * e_vec.y ;
    line[2] = rect.x + rect.width;
    line[3] = cy + ( (rect.x + rect.width - cx) / e_vec.x ) * e_vec.y;

    //need ``master" roi to put the line in the right spot
    line = {line[0]+roi.x,line[1]+roi.y,line[2]+roi.x,line[3]+roi.y};

  }

  
  std::pair<double,double> closest_point_on_line(std::vector<double>& line,int lx,int ly) {
    
    //real time collision detection page 128
    //closest point on line
    auto ax = line[0];
    auto ay = line[1];
    auto bx = line[2];
    auto by = line[3];
	
    auto abx = bx-ax;
    auto aby = by-ay;
	
    auto t = ( (lx-ax)*abx + (ly-ay)*aby ) / (abx*abx + aby*aby);
    auto dx = ax + t * abx;
    auto dy = ay + t * aby;

    return std::make_pair(dx,dy);
  }


  std::pair<double,double> get_mean_loc(const ::cv::Rect& rect,
					const Contour_t&  pts ) {
    
    double mean_x = 0.0;
    double mean_y = 0.0;
    
    for(const auto& pt : pts) { mean_x += pt.x;  mean_y += pt.y; }
    
    return { mean_x / ( (double) pts.size() ) + rect.x,mean_y / ( (double) pts.size() ) + rect.y};
  }

  
  int get_charge_sum(const ::cv::Mat& subImg, const Contour_t& pts ) {

    int charge_sum = 0;
    for(const auto& pt : pts ) charge_sum += (int) subImg.at<uchar>(pt.y,pt.x);

    return charge_sum;
  }

  Point2D intersection_point(const std::vector<double>& l1, const std::vector<int> l2) {
    
    Point2D pt;

    auto& x1 = l1[0]; double x3 = l2[0]; 
    auto& x2 = l1[2]; double x4 = l2[2];
    auto& y1 = l1[1]; double y3 = l2[1];
    auto& y2 = l1[3]; double y4 = l2[3];
    
    auto denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    pt.x = ( (x1*y2 - y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4) ) / denom;
    pt.y = ( (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4) ) / denom;
    
    return pt;
    
  }

  bool check_angle(const PCABox& b1, const PCABox& b2) {
    
    auto& ax = b1.e_vec_.x;
    auto& ay = b1.e_vec_.y;

    auto& bx = b2.e_vec_.x;
    auto& by = b2.e_vec_.y;

    auto cos_angle =  ax*bx + ay*by;

    if ( std::acos(cos_angle) <= b1.angle_cut_ ) //angle between PCA lines //use acos since it returns smallest angle
      return true;

    return false;

  }

  
  bool intersect(const PCABox& b1, const PCABox& b2){ 
    //Does the other PCABox line intersect me?

    const auto& b1line = b1.line_;
    const auto& b2box  = b2.box_;
    
    auto ledge = intersection_point(b1line, { b2box.x,b2box.y,b2box.x,b2box.y + b2box.height});
    if ( ledge.y >= b2box.y && ledge.y <= b2box.y+b2box.height)
      return true;
	      
    auto redge = intersection_point(b1line, { b2box.x+b2box.width,b2box.y,b2box.x+b2box.width,b2box.y+b2box.height});
    if ( redge.y >= b2box.y && redge.y <= b2box.y+b2box.height)
      return true;
    
    auto tedge = intersection_point(b1line, { b2box.x,b2box.y+b2box.height,b2box.x+b2box.width,b2box.y+b2box.height});
    if ( tedge.x >= b2box.x && tedge.x <= b2box.x+b2box.width)
      return true;
    
    auto bedge = intersection_point(b1line, { b2box.x,b2box.y,b2box.x+b2box.width,b2box.y});
    if ( bedge.x >= b2box.x && bedge.x <= b2box.x+b2box.width)
      return true;
    
    return false;
  }

  bool compatible(const PCABox& b1, const PCABox& b2) {

    if ( ! check_angle(b1,b2) )
      return false;
    
    if ( ! intersect(b1,b2) )
      return false;

    if ( ! intersect(b2,b1) )
      return false;
    
    return true;
  }

  std::vector<PCABox> sub_divide(PCABox& box, short divisions) {
    
    std::vector<PCABox> subboxes; subboxes.reserve(divisions);
    
    //auto& subboxes = box.subboxes_;
    auto& thebox   = box.box_;
    
    auto dx = thebox.width  /  2;
    auto dy = thebox.height /  2;
    
    for(unsigned i = 0; i < 2; ++i) {
      for(unsigned j = 0; j < 2; ++j) {
	
	auto x  = i * dx;
	auto y  = j * dy;
	
	::cv::Rect r(x,y,dx,dy);
	std::vector<double> line; line.resize(4);
	  
	Contour_t inside_pts; inside_pts.reserve(box.pts_.size());
	
	for(auto& pt : box.pts_) 
	  if ( r.contains(pt) )
	    inside_pts.emplace_back(pt);
	  
	if ( inside_pts.size() == 0 ) 
	  { subboxes.emplace_back(r + thebox.tl()); continue; }
	
	if ( inside_pts.size() < box.subhits_cut_ ) 
	  { subboxes.emplace_back(r + thebox.tl()); continue; }
	
	Point2D e_vec, e_center;
	
	pca_line(inside_pts,thebox,r,line,e_vec,e_center);
	
	subboxes.emplace_back(e_vec,e_center,line,inside_pts,
			      r + thebox.tl(),thebox,
			      box.angle_cut_,box.cov_cut_,box.subhits_cut_); //passing these parameters around is such a waste
	
	box.subdivided_ = true;
      }
    }

    return subboxes;
  }
  
  
  int decide_axis(std::vector<PCABox>& boxes, std::map<int,std::vector<int> > connections) {

    std::vector<PCAPath> paths; paths.resize(connections.size());
    
    int counter = 0;
    
    for( const auto& index: connections ) {
      auto& path = paths[counter];
      path.seed_ = index.first;
      
      path.push_back(&boxes[index.first]);
      
      for( const auto& c : index.second)
	
	path.push_back(&boxes[c]);
      
      path.Fill();
      
      ++counter;
      
    }
    
    std::sort( paths.begin(), paths.end(), []( const PCAPath& l, const PCAPath& r )
	       {
		 return l.cw_cov_ < r.cw_cov_ ;
	       });
    
    return paths.back().seed_;
  }
  
}

#endif
