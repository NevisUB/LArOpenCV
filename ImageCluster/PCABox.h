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
      e_vec_   ( e_vec    ),
      e_center_( e_center ),
      cov_     ( cov      ),
      box_     ( box      )
    {}
    
    //destructor
    ~PCABox(){}

    //not wasting time with std::pair sorry
    Point2D e_vec_;
    Point2D e_center_;

    double cov_;
    
    ::cv::Rect box_;

    bool intersect(const PCABox& other) const;
    
  };

}
#endif
