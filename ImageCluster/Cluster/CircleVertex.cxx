#ifndef __CIRCLEVERTEX_CXX__
#define __CIRCLEVERTEX_CXX__

#include "CircleVertex.h"
#include "PCACandidatesData.h"

namespace larocv {


  void CircleVertex::_Configure_(const ::fcllite::PSet &pset)
  {

  }
  
  larocv::Cluster2DArray_t CircleVertex::_Process_(const larocv::Cluster2DArray_t& clusters,
						   const ::cv::Mat& img,
						   larocv::ImageMeta& meta,
						   larocv::ROI& roi) {

    
    const auto& pcacandidates_data = AlgoData<PCACandidatesData>(ID()-1);
    const auto& ipoints_v_v = pcacandidates_data._ipoints_v_v_v[meta.plane()]; // intersection points, 1 per initial contour.
    
    
    return clusters;
  }
  
  
  
}
#endif
