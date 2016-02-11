#ifndef __PCABOX_CXX__
#define __PCABOX_CXX__

#include "PCABox.h"

namespace larcv {

  //this checks ONE big box against another...
  bool PCABox::compatible(const PCABox& other) const {

    if ( ! this->check_angle(other) )
      return false;

    if ( ! this->intersect(other) )
      return false;

    if ( ! other.intersect(*this) )
      return false;

    return true;
  }
  
  
  bool PCABox::check_angle(const PCABox& other) const {

    auto& ax = this->e_vec_.x;
    auto& ay = this->e_vec_.y;

    auto& bx = other.e_vec_.x;
    auto& by = other.e_vec_.y;

    auto cos_angle =  ax*bx + ay*by;

    if ( angle_cut_ < 0 ) { std::cout << "Call SetAngleCut()\n"; throw std::exception(); }
    
    if ( std::acos(cos_angle) <= angle_cut_ ) //angle between PCA lines //use acos since it returns smallest angle
      return true;

    return false;

  }

  
  bool PCABox::intersect(const PCABox& other) const {
    //Does the other PCABox line intersect me?
    const auto& obox = other.box_;

    auto ledge = intersection_point(line_, { obox.x,obox.y,obox.x,obox.y + obox.height});
    if ( ledge.y >= obox.y && ledge.y <= obox.y+obox.height)
      return true;
	      
    auto redge = intersection_point(line_, { obox.x+obox.width,obox.y,obox.x+obox.width,obox.y+obox.height});
    if ( redge.y >= obox.y && redge.y <= obox.y+obox.height)
      return true;
    
    auto tedge = intersection_point(line_, { obox.x,obox.y+obox.height,obox.x+obox.width,obox.y+obox.height});
    if ( tedge.x >= obox.x && tedge.x <= obox.x+obox.width)
      return true;
    
    auto bedge = intersection_point(line_, { obox.x,obox.y,obox.x+obox.width,obox.y});
    if ( bedge.x >= obox.x && bedge.x <= obox.x+obox.width)
      return true;
    
    return false;
  }
  
  void PCABox::SubDivide(short divisions) {

    subboxes_.reserve(divisions);
    
    auto dx = box_.width  /  2;
    auto dy = box_.height /  2;

    for(unsigned i = 0; i < 2; ++i) {
      for(unsigned j = 0; j < 2; ++j) {
	
	auto x  = i * dx;
	auto y  = j * dy;
	
	::cv::Rect r(x,y,dx,dy);
	std::vector<double> line; line.resize(4);
	  
	Contour_t inside_pts; inside_pts.reserve(pts_.size());
		
	for(auto& pt : pts_) 
	  if ( r.contains(pt) )
	    inside_pts.emplace_back(pt);
	  
	if ( inside_pts.size() == 0 ) 
	  { subboxes_.emplace_back(PCABox()); continue; }
	
	if ( inside_pts.size() <= 2 ) // hardcode warning must have more than 3 hits or this is useless
	  { subboxes_.emplace_back(PCABox()); continue; }
	
	Point2D e_vec;
	Point2D e_center;
	
	auto q = pca_line(inside_pts,box_,r,line,e_vec,e_center);
	
	auto cov = get_roi_cov(inside_pts);
	subboxes_.emplace_back(e_vec,e_center,cov,line,inside_pts,r + box_.tl());

	subdivided_ = true;
      }
    }

  }

  bool PCABox::touching(const PCABox& other) const {

    auto overlap = dbox_ & other.box_;
    
    if ( overlap.area() )
      return true;
		 
    return false;

  }

  void PCABox::expand(short i, short j) {
    
    dbox_ = box_ - ::cv::Point(i,j);
    dbox_ += ::cv::Size(2*i,2*j);
    
  }
  
}
#endif
