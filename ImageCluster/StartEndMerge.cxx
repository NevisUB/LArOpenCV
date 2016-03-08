//by vic
#ifndef __STARTENDMERGE_CXX__
#define __STARTENDMERGE_CXX__

#include "StartEndMerge.h"

namespace larcv{

  void StartEndMerge::_Configure_(const ::fcllite::PSet &pset)
  {
    _max_start_end_d = pset.get<double>("MaxStartEndDistance");
 
    _w = 0.0557;
    _h = 0.3;
  }

  Cluster2DArray_t StartEndMerge::_Process_(const larcv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larcv::ImageMeta& meta)
  { 
    Cluster2DArray_t oclusters; oclusters.reserve(cluster.size());


    // we should just traverse graph. Start at one end point, try and jump to another cluster, if next cluster
    // end point is close to another, jump to it, if not reel back one and jump again until all connections are
    // made. Then merge the cluster (somehow...)
    
    return oclusters;
  }

}
#endif
