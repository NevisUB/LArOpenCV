#ifndef __VERTEXINHULL_CXX__
#define __VERTEXINHULL_CXX__

#include "VertexInHull.h"

namespace larocv{

  /// Global larocv::VertexInHullFactory to register AlgoFactory
  static VertexInHullFactory __global_VertexInHullFactory__;
  
  void VertexInHull::_Configure_(const Config_t &pset)
  {}

  Cluster2DArray_t VertexInHull::_Process_(const larocv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larocv::ImageMeta& meta, larocv::ROI& roi)
  { 

    // Basically Vic's Convex hull alg.  
    // Adds check on vertex in convex hull-- remove if true
  
    Cluster2DArray_t oclusters;
    oclusters.reserve( clusters.size() );
    
    std::vector<std::vector<int> > hullpts_v;
    hullpts_v.resize(clusters.size());

    Point2D vtx_in_plane = roi.roivtx_in_image(meta); 
    ::cv::Point2f vtx (vtx_in_plane.x,vtx_in_plane.y);
    
    for (unsigned i = 0; i < clusters.size(); ++i ) { 

      auto& cluster = clusters [i];
      auto& hullpts = hullpts_v[i];
    
      ::cv::convexHull(cluster._contour, hullpts);
      hullpts.push_back(hullpts.at(0)); // odd that this is needed to close up the hull

      std::vector<::cv::Point> hullcontour;
      hullcontour.resize(hullpts.size()-1);

      for(unsigned u=0;u<hullcontour.size();++u) 
        hullcontour[u] = cluster._contour[ hullpts[u] ];

      if( ::cv::pointPolygonTest(hullcontour,vtx,false) > 0 ) continue;
      
      oclusters.emplace_back(cluster);

      }
  
  return oclusters; }

}
#endif
