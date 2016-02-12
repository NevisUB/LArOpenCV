//by vic
#ifndef __PCAPATH_H__
#define __PCAPATH_H__

#include "PCABox.h"

namespace larcv {

  class PCAPath : public std::vector<PCABox*> {

  public:

    PCAPath() :
      total_area_(0),
      avg_area_(0),
      charge_sum_(0),
      cw_covariance_(0),
      cw_slope_(0),
      cw_area_(0),
      n_points_(0),
      point_density_(0)
    {}

    ~PCAPath(){}

    size_t seed_;
    int    n_points_;
    
    double total_area_;
    double avg_area_;
    double charge_sum_;
    double cw_covariance_;
    double cw_slope_;
    double cw_area_;

    double point_density_;
    
    
    void Fill();
    
  };
}
#endif
/** @} */ // end of doxygen group 

