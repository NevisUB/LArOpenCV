#ifndef __CIRCLEVERTEX_CXX__
#define __CIRCLEVERTEX_CXX__

#include "CircleVertex.h"
#include "PCACandidatesData.h"

#include "Core/Circle.h"

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

    auto& circlevertex_data = AlgoData<larocv::CircleVertexData>();
    
    //lets make minimum enclosing circle, take it to python to see it
    for(auto& ipoints_v : ipoints_v_v) {
      geo2d::Circle<float> circ(ipoints_v);
      circlevertex_data._circledata_v_v[meta.plane()].emplace_back(std::move(circ));
    }
    
    
    return clusters;
  }
  
  
  
}
#endif
