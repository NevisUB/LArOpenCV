//by vic
#ifndef __PCAPATH_CXX__
#define __PCAPATH_CXX__

#include "PCAPath.h"
#include "PCAUtilities.h"
#include "StatUtilities.h"

namespace larcv {

  // just calculate everything who knows what we will need
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

	
      avg_center_pca.x += box->e_center_.x + box->parent_roi_.x;
      avg_center_pca.y += box->e_center_.y + box->parent_roi_.y;

      cw_center_pca.x += charge_sum * ( box->e_center_.x + box->parent_roi_.x );
      cw_center_pca.y += charge_sum * ( box->e_center_.y + box->parent_roi_.y );
      
    }

    double nboxes = (double) this->size(); 

    avg_area_   = total_area_   / nboxes;
    avg_cov_    = total_cov_    / nboxes;
    avg_slope_  = total_slope_  / nboxes;
    avg_points_ = total_points_ / nboxes;
    avg_d_pca_  = total_d_pca_  / (double) total_points_;

    avg_center_pca.x /= nboxes;
    avg_center_pca.y /= nboxes;

    cw_center_pca.x /= total_charge_;
    cw_center_pca.y /= total_charge_;
    
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
    
    pca_line(combined,combined_e_vec_,combined_e_center_);
    
    for (const auto& pt : combined )
      combined_total_d_pca_ += roi_d_to_line(combined_e_vec_,combined_e_center_,pt.x,pt.y);

    combined_avg_d_pca_ = combined_total_d_pca_ / ( (double) combined.size() );
					     
  }

  void PCAPath::CheckMinAreaRect(const ::cv::RotatedRect& rr) {

    size_t far_from_center;
    double d = 0.0;
    for(size_t i=0;i<this->size();++i) {
      auto& box = this->at(i);
      
      ::cv::Point2f bcenter( box->e_center_.x + box->parent_roi_.x, 
			     box->e_center_.y + box->parent_roi_.y );

      auto dd = dist(rr.center,bcenter);
      if ( dd > d ) { d = dd; far_from_center = i; }
    }
    
    //the fuck? PCA boxes get destructed BEFORE I try to access them in python?
    //copy it into public var in PCAPath.... im stupid
    far_from_center_ = this->at(far_from_center)->box_;
    e_vec_far_       = this->at(far_from_center)->e_vec_;


    //inside this box, get the closest point to the edge...
    for( const auto& pt : this->at(far_from_center)->pts_ ) {

      
    }
    
  }


  
}


#endif
