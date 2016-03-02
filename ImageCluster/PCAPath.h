//by vic
#ifndef __PCAPATH_H__
#define __PCAPATH_H__

#include "PCABox.h"

namespace larcv {

  class PCAPath : public std::vector<PCABox*> {

  public:

    PCAPath()
    {}

    ~PCAPath(){}

    size_t seed_;


    /////////*********/////////
    void Fill();

    double total_area_;
    double avg_area_;
    double cw_area_;
    
    double total_charge_;
    double avg_charge_;

    double total_cov_;
    double avg_cov_;
    double cw_cov_;

    double total_slope_;
    double avg_slope_;
    double cw_slope_;

    int total_points_;
    int avg_points_;
    
    double point_density_;

    double total_d_pca_;
    double avg_d_pca_;
    double cw_d_pca_;

    Point2D avg_center_pca;
    Point2D cw_center_pca;
    /////////*********/////////
    
    /////////*********/////////
    void CombinedPCA();
    double combined_cov_;
    double combined_total_d_pca_;
    double combined_avg_d_pca_;
    double combined_cw_d_pca_;


    Point2D combined_e_vec_;
    Point2D combined_e_center_;
    /////////*********/////////
    

    /////////*********/////////
    void CheckMinAreaRect(const ::cv::RotatedRect& rr,
			  const std::vector<::cv::Point2f>& pts);
    
    ::cv::Rect far_from_center_;
    Point2D e_vec_far_;
    Point2D point_closest_to_edge_;
    /////////*********/////////

    std::vector<::cv::Rect> chosen_boxes_;
    
    
  };
}
#endif
/** @} */ // end of doxygen group 

