#ifndef __EMPTYIMAGECLUSTER_CXX__
#define __EMPTYIMAGECLUSTER_CXX__

#include "EmptyImageCluster.h"

namespace larocv{

  void EmptyImageCluster::_Configure_(const ::fcllite::PSet &pset)
  {
  }

  Cluster2DArray_t EmptyImageCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta, larocv::ROI& roi)
  { 

    //LAROCV_INFO() << "NAMERU JA NAI\n";
    return Cluster2DArray_t();
  
   }  
  
  

}
#endif
