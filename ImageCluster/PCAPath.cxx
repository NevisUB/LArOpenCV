//by vic
#ifndef __PCAPATH_CXX__
#define __PCAPATH_CXX__

#include "PCAPath.h"

namespace larcv {

  void PCAPath::Fill() {
    
    for(const auto& box : *this ) {

      total_area_    += box->box_.area();
      charge_sum_    += box->charge_sum_;

      cw_covariance_ += box->charge_sum_ * box->cov_;
      cw_slope_      += box->charge_sum_ * ( box->line_[3] - box->line[1] ) / ( box->line_[2] - box->line[0] );
      cw_area_       += box->charge_sum_ * box->box_.area();

      n_points_ += box->pts_.size();
    }


    avg_area_      = total_area_ / ( (double) this->size() );
    point_density_ = ( (double) n_points_ ) / total_area_;
    
    cw_covariance_ /= charge_sum_;
    cw_slope_      /= charge_sum_;
    cw_area_       /- charge_sum_;
  }
  
}


#endif
