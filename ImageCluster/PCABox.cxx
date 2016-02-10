#ifndef __PCABOX_CXX__
#define __PCABOX_CXX__

#include "PCABox.h"
namespace larcv {

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

    //This
    auto cx =this->e_center_.x + this->box_.x;
    auto cy =this->e_center_.y + this->box_.y;

    auto& ex = this->e_vec_.x; 
    auto& ey = this->e_vec_.y;
    
    //The other box

    //bottom, top
    auto& oyb = other.box_.y;
    auto  oyt = other.box_.y + other.box_.height;

    //left right
    auto& oxl = other.box_.x;
    auto  oxr = other.box_.x + other.box_.width;

    double ft, iy, ix;
    
    //first side
    ft = (oxl - cx) / ex;
    iy = cy + ey * ft;
    
    if ( iy <= oyt && iy >= oyb )
      return true;

    //second side
    ft = (oxr - cx) / ex;
    iy = cy + ey * ft;
    
    if ( iy <= oyt && iy >= oyb )
      return true;

    //top
    ft = (oyt - cy) / ey;
    ix = cx + ex * ft;
    
    if ( ix <= oxr && iy >= oxl )
      return true;

    //bottom
    ft = (oyb - cy) / ey;
    ix = cx + ex * ft;
    
    if ( ix <= oxr && iy >= oxl )
      return true;
    
    
    return false;
  }
  

}
#endif
