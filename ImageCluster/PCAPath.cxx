//by vic
#ifndef __PCAPATH_CXX__
#define __PCAPATH_CXX__

#include "PCAPath.h"
#include "PCAUtilities.h"

namespace larcv {

  void PCAPath::Fill() {
    
    for(const auto& box : *this ) {

      total_area_ += box->box_.area();
      cw_area_    += box->charge_sum_ * box->box_.area();
      
      total_charge_ += box->charge_sum_;
      
      total_cov_ += std::abs( box->cov_ );
      cw_cov_    += box->charge_sum_ * std::abs( box->cov_ );

      auto  slope = ( box->line_[3] - box->line_[1] ) / ( box->line_[2] - box->line_[0] );
      total_slope_   += slope;
      cw_slope_      += box->charge_sum_ * slope;
      
      total_points_ += box->pts_.size();
      
    }

    double nboxes = (double) this->size(); 

    avg_area_   = total_area_   / nboxes;
    avg_cov_    = total_cov_    / nboxes;
    avg_slope_  = total_slope_  / nboxes;
    avg_points_ = total_points_ / nboxes;
    
    cw_area_   /= total_charge_;
    // cw_cov_    /= total_charge_;
    cw_slope_  /= total_charge_;

    point_density_ = nboxes / total_area_;
    
  }


  void PCAPath::CombinedPCA() {

    Contour_t combined;

    for( const auto& box : *this ) 
      for( auto& pt : box->pts_ )
	combined.emplace_back(pt);
    
    Point2D e_vec, e_center;
    pca_line(combined,e_vec,e_center);
    
    
  }
  
}


#endif
