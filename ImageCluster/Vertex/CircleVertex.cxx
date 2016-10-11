#ifndef __CIRCLEVERTEX_CXX__
#define __CIRCLEVERTEX_CXX__

#include "CircleVertex.h"

namespace larocv {


  void CircleVertex::_Configure_(const ::fcllite::PSet &pset)
  {
    
  }
  
  larocv::Cluster2DArray_t CircleVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
						   const ::cv::Mat& img,
						   larocv::ImageMeta& meta,
						   larocv::ROI& roi)
  {
    //find the intersection points of the PCAs per clusters
    //it's current stored in unfortunately bloated Cluster2D object
    //as eigenVecFirst.
    
    for(const auto& cluster : clusters) {
    }
    
    return clusters;
  }

  

}
#endif
