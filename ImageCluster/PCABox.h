//by vic
#ifndef __PCABOX_H__
#define __PCABOX_H__

#include <iostream>
#include <cmath>
#include <vector>

#include "Core/ImageMeta.h"
#include "Core/laropencv_base.h"

#include "ImageClusterTypes.h"

namespace larcv {
  
  class PCABox {

  public:
    
    PCABox() {}
    
    PCABox(::cv::Rect box) :
      box_       ( box   ),
      empty_     ( true  ),
      subdivided_( false )
    {}
    
    PCABox(Point2D e_vec,
	   Point2D e_center,
	   double  cov,
	   std::vector<double> line,
	   Contour_t points, // this should be reference(!)
	   ::cv::Rect box,
	   ::cv::Rect parent_roi,
	   double angle_cut,
	   double cov_cut,
	   int subhits_cut) :
      e_vec_      ( e_vec       ),
      e_center_   ( e_center    ),
      cov_        ( cov         ),
      line_       ( line        ),
      pts_        ( points      ),
      box_        ( box         ),
      parent_roi_ ( parent_roi  ),
      angle_cut_  ( angle_cut   ),
      cov_cut_    ( cov_cut     ),
      subhits_cut_( subhits_cut ),
      charge_sum_ ( 0           ),
      subdivided_ ( false       ),
      empty_      ( false       )
    {}
    
    //destructor
    ~PCABox(){}
    
    //not wasting time with std::pair sorry
    Point2D e_vec_;
    Point2D e_center_;
    
    double cov_;
    
    std::vector<double> line_;
    
    Contour_t pts_;
    
    ::cv::Rect box_;
    ::cv::Rect parent_roi_;
    
    ::cv::Rect dbox_; //dilated box for intersection
    
    double angle_cut_;
    double cov_cut_;
    int    subhits_cut_;
    
    void expand(short i, short j);

    // void SetChargeSum  (double c) { charge_sum_= c; }
    
    // only filled if subdivided
    // void SubDivide(short divisions);
    bool subdivided_;
    // std::vector<PCABox> subboxes_;
    bool empty_;

    double charge_sum_;
    // should we use polymorphism here? It's hard to say
    // since PCASegmentation object is created by factory.
    // So to specify PCABox child at runtime means
    // we probably need {\it another} factory to create
    // PCABox instance just to abstract compatible/touching function.
    // Let's not waste time, just do it
    
    // bool compatible(const PCABox& other) const;
    bool touching  (const PCABox& other) const;
		  
  private:
    //what you want to check in compatible function,
    //just add function here until we abstract (will move to utilities)


    //int n_hits_;
  };

}
#endif
