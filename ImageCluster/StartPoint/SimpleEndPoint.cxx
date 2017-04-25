#ifndef __SIMPLEENDPOINT_CXX__
#define __SIMPLEENDPOINT_CXX__

#include "SimpleEndPoint.h"

namespace larocv {

  void SimpleEndPoint::_Configure_(const ::fcllite::PSet &pset)
  {
  }

  Cluster2DArray_t SimpleEndPoint::_Process_(const larocv::Cluster2DArray_t& clusters,
					     const ::cv::Mat& img,
					     larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    Cluster2DArray_t oclusters;

    for(auto& cluster : clusters) {

      float d = 0;
      int idx = -1;
      
      for(unsigned i=0;i<cluster._insideHits.size();++i){
	auto& ihit = cluster._insideHits[i];
	float dd = dist(ihit,cluster._startPt);
	if ( dd > d )
	  { d = dd; idx = i; }
      }
      if (idx < 0) continue;//throw larbys("why (SimpleEndPoint)");

      oclusters.push_back(cluster);
      
      oclusters.back()._endPt = Point2D(cluster._insideHits[idx].x,
					cluster._insideHits[idx].y);
    }

    return oclusters;
  }


  float SimpleEndPoint::dist(const ::cv::Point& pt1, const Point2D& pt2) {
    //explicitly make them floats
    float dx = pt1.x - pt2.x;
    float dy = pt1.y - pt2.y;
    
    return std::sqrt(dx*dx + dy*dy);
    
  }

}
#endif
