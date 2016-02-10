#ifndef __PCABOX_H__
#define __PCABOX_H__

#include <iostream>
#include <cmath>
#include <vector>

#include "Core/ImageMeta.h"
#include "Core/laropencv_base.h"

namespace larcv {
  
  class PCABox {

  public:
    
    //needed default ctor
    PCABox() {}
    
    PCABox(Point2D e_vec,
	   Point2D e_center,
	   double  cov,
	   ::cv::Rect box) :
      e_vec_    ( e_vec    ),
      e_center_ ( e_center ),
      cov_      ( cov      ),
      box_      ( box      ),
      angle_cut_( -1       )
    {}
    
    //destructor
    ~PCABox(){}

    //not wasting time with std::pair sorry
    Point2D e_vec_;
    Point2D e_center_;

    double cov_;
    
    ::cv::Rect box_;

    void SetAngleCut(double a) { angle_cut_ = a; }
    
    // should we use polymorphism here? It's hard to say
    // since PCASegmentation object is created by factory.
    // So to specify PCABox child and runtime means
    // we probably need {\it another} factory to create
    // PCABox instance. Let's not waste time
    bool compatible(const PCABox& other) const;
    

  private:
    //what you want to check, just add function here until we abstract
    bool intersect    (const PCABox& other) const;
    bool check_angle  (const PCABox& other) const;

    double angle_cut_;
    
  };

}
#endif
