//by vic
#ifndef __PCAPATH_CXX__
#define __PCAPATH_CXX__

#include "PCAPath.h"
#include "PCAUtilities.h"
#include "StatUtilities.h"

namespace larcv {

  void PCAPath::Fill() {
    
    for(const auto& box : *this ) {

      auto charge_sum = box->charge_sum();
      total_charge_ += charge_sum;
      
      total_area_ += box->box_.area();
      cw_area_    += charge_sum * box->box_.area();

      total_cov_ += std::abs( box->cov_ );
      cw_cov_    += charge_sum * std::abs( box->cov_ );

      auto  slope = ( box->line_[3] - box->line_[1] ) / ( box->line_[2] - box->line_[0] );
      total_slope_   += slope;
      cw_slope_      += charge_sum * slope;
      
      total_points_ += box->pts_.size();

      total_d_pca_ += total_d_pca   ( *box );
      cw_d_pca_    += total_cw_d_pca( *box );
            
    }

    double nboxes = (double) this->size(); 

    avg_area_   = total_area_   / nboxes;
    avg_cov_    = total_cov_    / nboxes;
    avg_slope_  = total_slope_  / nboxes;
    avg_points_ = total_points_ / nboxes;
    avg_d_pca_  = total_d_pca_  / (double) total_points_;

    cw_area_   /= total_charge_;
    // cw_cov_    /= total_charge_;
    cw_slope_  /= total_charge_;
    cw_d_pca_  /= total_charge_;
    
    point_density_ = nboxes / total_area_;

  }


  void PCAPath::CombinedPCA() {

    Contour_t combined;

    for( const auto& box : *this ) 
      for( auto& pt : box->pts_ )
	combined.emplace_back(pt + box->parent_roi_.tl());


    combined_cov_ = roi_cov(combined);
    
    Point2D e_vec, e_center;
    pca_line(combined,e_vec,e_center);

    for (const auto& pt : combined )
      combined_total_d_pca_ += roi_d_to_line(e_vec,e_center,pt.x,pt.y);

    combined_avg_d_pca_ = combined_total_d_pca_ / ( (double) combined.size() );
					     
  }
  
}


#endif
