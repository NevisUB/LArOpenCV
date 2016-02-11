//by vic
#ifndef __PCABOX_H__
#define __PCABOX_H__

#include <iostream>
#include <cmath>
#include <vector>

#include "Core/ImageMeta.h"
#include "Core/laropencv_base.h"
#include "PCAUtilities.h"


namespace larcv {
  
  class PCABox {

  public:
    
    //needed default ctor
    PCABox() : empty_(true), subdivided_(false) {}
    
    PCABox(Point2D e_vec,
	   Point2D e_center,
	   double  cov,
	   std::vector<double> line,
	   Contour_t points, // this should be reference(!)
	   ::cv::Rect box) :
      e_vec_     ( e_vec    ),
      e_center_  ( e_center ),
      cov_       ( cov      ),
      line_      ( line     ),
      pts_       ( points   ),
      box_       ( box      ),
      angle_cut_ ( -1       ),
      n_hits_    ( -1       ),
      charge_sum_( -1       ),
      subdivided_( false    ),
      empty_     ( false    )
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
    ::cv::Rect dbox_;


    
    void expand(short i, short j);

    void SetAngleCut   (double a) { angle_cut_ = a; }
    void SetChargeSum  (double c) { charge_sum_= c; }
    void SetNHits      (int    h) { n_hits_    = h; }

    // only filled if subdivided
    void SubDivide(short divisions);

    bool subdivided_;

    std::vector<PCABox> subboxes_;

    bool empty_;
    
    // { 0 == bl,1 == tl,2 == br,4 == tr }


    // should we use polymorphism here? It's hard to say
    // since PCASegmentation object is created by factory.
    // So to specify PCABox child and runtime means
    // we probably need {\it another} factory to create
    // PCABox instance. Let's not waste time
    bool compatible(const PCABox& other) const;
    bool touching  (const PCABox& other) const;
		  
  private:
    //what you want to check, just add function here until we abstract
    bool intersect    (const PCABox& other) const;
    bool check_angle  (const PCABox& other) const;

    
    double angle_cut_;
    double charge_sum_;
    int n_hits_;
  };

}
#endif
