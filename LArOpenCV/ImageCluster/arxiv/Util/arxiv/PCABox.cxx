
//by vic
#ifndef __PCABOX_CXX__
#define __PCABOX_CXX__

#include "PCABox.h"


namespace larocv {
  
  bool PCABox::touching(const PCABox& other) const {
    
    auto overlap = dbox_ & other.box_;
    
    if ( overlap.area() )

      return true;

    return false;

  }

  void PCABox::expand(short i, short j) {
    
    dbox_  = box_ - ::cv::Point(i,j);
    dbox_ += ::cv::Size(2*i,2*j);
    
  }

}
#endif
