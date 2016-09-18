#ifndef __IOUOVERLAP_CXX__
#define __IOUOVERLAP_CXX__

#include "IoUOverlap.h"

namespace larocv{

  void IoUOverlap::_Configure_(const ::fcllite::PSet &pset)
  {
    
  }
  
  double IoUOverlap::_Process_(const larocv::Cluster2DPtrArray_t& clusters, const std::vector<double>& vtx)
  {
        

    double t_min_abs = 9600; // smallest start point of the 3
    double t_max_abs = 0;    // largest start point of the three
        
    for(auto const& c : clusters){
      
      double min(0.0), max(0.0);
      
      getMinMaxTime(c,min,max);
      
      if ( min < t_min_abs )
	t_min_abs = min;
      if ( max   > t_max_abs )
	t_max_abs = max;
      
    }
    
    if (clusters.size() < 2) return -1;

    // do the clusters overlap at all?
    bool overlap = true;

    double t_min_common(0.0), t_max_common(0.0);

    getMinMaxTime(clusters[0],t_min_common,t_max_common);

    for (size_t i=1; i < clusters.size(); i++){

      double t_min(0.0), t_max(0.0); // min and max for current cluster
      getMinMaxTime(clusters[i],t_min,t_max);

      // now find overlap
      if (t_max < t_min_common){
	overlap = false;
	break;
      }

      if (t_min > t_max_common){
	overlap = false;
	break;
      }

      if (t_min > t_min_common) t_min_common = t_min;
      if (t_max < t_max_common) t_max_common = t_max;

    }// for all clusters

    if (overlap == false) return -1;

    // calculate overlap

    double iou = (t_max_common - t_min_common) / (t_max_abs - t_min_abs);

    return iou;
  }
  
  void IoUOverlap::getMinMaxTime(const Cluster2D* cluster, double& min, double& max)
  {
    
    auto const& hits = cluster->_insideHits;
    
    min = 9600;
    max = 0;
    
    for (auto const& hit : hits){
      auto t = _pixel_y(cluster,hit.x);
      
      if (t > max) max = t;
      if (t < min) min = t;
      
    }// for all hits
    
    return;
  }

  double IoUOverlap::_pixel_y(const Cluster2D* cluster,size_t pix) {
    auto const& px_h   = cluster->PixelHeight();
    auto const& origin = cluster->Origin();
    
    return (pix+0.5)*px_h + origin.y;
  }
  
}
#endif
