#ifndef __BOUNDINGRECTDIR_CXX__
#define __BOUNDINGRECTDIR_CXX__

#include "BoundingRectDir.h"

namespace larocv{

  /// Global larocv::BoundingRectDirFactory to register AlgoFactory
  static BoundingRectDirFactory __global_BoundingRectDirFactory__;

  void BoundingRectDir::_Configure_(const ::fcllite::PSet &pset)
  {}
  
  Cluster2DArray_t BoundingRectDir::_Process_(const larocv::Cluster2DArray_t& clusters,
					      const ::cv::Mat& img,
					      larocv::ImageMeta& meta, larocv::ROI& roi)
  {
    
    auto oclusters = clusters;
    for (auto& ocluster : oclusters) {
      
      //get the verticies for the bounding rectangle
      auto& verts = ocluster._minAreaRect;

      float d=-1;
      short idx1 = -1;
      short idx2 = -1;
      
      //get the two indicies of the vertex for the longest side
      for(short i=0;i<3;++i) {
	auto& p1 = verts[i];
	auto& p2 = verts[i+1];

	auto dd = dist(p1,p2);

	if (dd > d) {
	  d = dd;
	  idx1 = i;
	  idx2 = i+1;
	}
      }

      //check the last point
      auto dd = dist(verts[3],verts[0]);
      if (dd > d) {
	d = dd;
	idx1 = 3;
	idx2 = 0;
      }

      float dx = verts[idx2].x - verts[idx1].x;
      float dy = verts[idx2].y - verts[idx1].y;
      float mag = std::sqrt(dx*dx+dy*dy);

      ocluster.reco.dir=Point2D(dx/mag,dy/mag);
      
    }
    return oclusters;
  }



  float BoundingRectDir::dist(const ::cv::Point2f& p1, const ::cv::Point2f& p2) {
    return std::sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
  }
			      
}



#endif
