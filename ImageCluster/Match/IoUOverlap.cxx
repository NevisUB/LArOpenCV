#ifndef __IOUOVERLAP_CXX__
#define __IOUOVERLAP_CXX__

#include "IoUOverlap.h"

namespace larocv{

  void IoUOverlap::_Configure_(const ::fcllite::PSet &pset)
  {
  _iou_score = pset.get<float> ("IOUScore");
  _hit_ratio = pset.get<float> ("HitRatio");
  }
  
  double IoUOverlap::_Process_(const larocv::Cluster2DPtrArray_t& clusters, const std::vector<double>& vtx)
  {
        

    double t_min_abs = 9600; // smallest start point of the 3
    double t_max_abs = 0;    // largest start point of the three

    float max_hits = 0;
    float min_hits = 1e9;

    for(auto const& c : clusters){
      
      double min(0.0), max(0.0);
      
      getMinMaxTime(c,min,max);
      
      if ( min < t_min_abs )
	t_min_abs = min;
      if ( max   > t_max_abs )
	t_max_abs = max;

      // Also find max and min hits
      if ( c->_insideHits.size() > max_hits )
        max_hits = c->_insideHits.size();

      if ( c->_insideHits.size() < min_hits )
        min_hits = c->_insideHits.size();

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

    // If one cluster has < 10% of the hits of the other cluster, ignore this pair
    double hit_frac = min_hits / max_hits;

    if (overlap == false || hit_frac < _hit_ratio ) return -1;

    // calculate overlap

    double iou = (t_max_common - t_min_common) / (t_max_abs - t_min_abs);

    //std::cout<<"Overlap is: "<<iou<<", "<<min_hits<<", "<<max_hits<<std::endl ;
    //for (size_t i=0; i < clusters.size(); i++)
    //  std::cout<<"Plane: "<<clusters[i]->PlaneID()<<", Hits: "<<clusters[i]->_numHits() <<std::endl ;

    if ( iou < _iou_score ) return -1;


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
