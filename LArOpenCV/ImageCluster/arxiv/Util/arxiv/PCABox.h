//by vic
#ifndef __PCABOX_H__
#define __PCABOX_H__

#include <iostream>
#include <cmath>
#include <vector>
#include <numeric>

#include "LArOpenCV/Core/ImageMeta.h"
#include "LArOpenCV/Core/laropencv_base.h"

#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"

#include "StatUtilities.h"

namespace larocv {
  
  class PCABox {

  public:
    
    PCABox() {}
    
    PCABox(::cv::Rect box) :
      box_       ( box   ),
      subdivided_( false ),
      empty_     ( true  )
    {}
    
    PCABox(Point2D e_vec,
	   Point2D e_center,
	   std::vector<double> line,
	   Contour_t points, // this should be reference(!)
	   ::cv::Rect box,
	   ::cv::Rect parent_roi,
	   double angle_cut,
	   double cov_cut,
	   int subhits_cut) :
      e_vec_      ( e_vec       ),
      e_center_   ( e_center    ),
      line_       ( line        ),
      pts_        ( points      ),
      box_        ( box         ),
      parent_roi_ ( parent_roi  ),
      angle_cut_  ( angle_cut   ),
      cov_cut_    ( cov_cut     ),
      subhits_cut_( subhits_cut ),
      subdivided_ ( false       ),
      empty_      ( false       )
    {
      cov_      = roi_cov  (pts_);
    }
    
    //destructor
    ~PCABox(){}
    
    //not wasting time with std::pair sorry
    Point2D e_vec_;
    Point2D e_center_;
    
    double cov_;
    
    std::vector<double> line_;

    std::vector<int> charge_;
    int charge_sum() const { return std::accumulate(charge_.begin(),charge_.end(),0); }
    
    Contour_t pts_;
    
    ::cv::Rect box_;
    ::cv::Rect parent_roi_;
    ::cv::Rect dbox_; //dilated box for intersection condition
    
    double angle_cut_;
    double cov_cut_;
    int    subhits_cut_;
    
    Point2D cw_center_;
    Point2D avg_center_;
    
    void expand(short i, short j);

    // void SetChargeSum  (double c) { charge_sum_= c; }
    
    // only filled if subdivided
    // void SubDivide(short divisions);
    bool subdivided_;
    // std::vector<PCABox> subboxes_;
    bool empty_;

    // should we use polymorphism here? It's hard to say
    // since PCASegmentation object is created by factory.
    // So to specify PCABox child at runtime means
    // we probably need {\it another} factory to create
    // PCABox instance just to abstract compatible/touching function.
    // Let's not waste time, just do it
    
    // bool compatible(const PCABox& other) const;
    bool touching  (const PCABox& other) const;
    
  protected:

  private:
    
  };

}
#endif
