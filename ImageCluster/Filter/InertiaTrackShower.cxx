#ifndef __INERTIATRACKSHOWER_CXX__
#define __INERTIATRACKSHOWER_CXX__

#include "InertiaTrackShower.h"

namespace larocv {

  void InertiaTrackShower::_Configure_(const ::fcllite::PSet &pset)
  {
  }
  
  Cluster2DArray_t InertiaTrackShower::_Process_(const larocv::Cluster2DArray_t& clusters,
						const ::cv::Mat& img,
						larocv::ImageMeta& meta, larocv::ROI& roi)
  {
    Cluster2DArray_t oclusters;
    oclusters.resize(clusters.size());

    ::cv::Moments moms;
    const double eps = 1e-2;
    for(const auto& cluster : clusters) {

      //for now lets calculate it here, if it's useful we will
      //put it into FillClusterParams, and place it in the header file
      moms = moments(::cv::Mat(cluster._contour));

      //ask that the demonimator isn't too small, when can this happen?
      double denominator = std::sqrt(std::pow(2 * moms.mu11, 2) + std::pow(moms.mu20 - moms.mu02, 2));
      
      double ratio;
      if (denominator > eps) {
	  double cosmin = (moms.mu20 - moms.mu02) / denominator;
	  double sinmin = 2 * moms.mu11 / denominator;
	  double cosmax = -cosmin;
	  double sinmax = -sinmin;

	  double imin = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmin - moms.mu11 * sinmin;
	  double imax = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmax - moms.mu11 * sinmax;
	  ratio = imin / imax;
	}
      else {
	ratio = 1;
      }
      
      std::cout << "ratio is ... " << ratio << "\n";
    }
    
    return clusters;
  }
  
}
#endif
