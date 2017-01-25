#ifndef __MINNCLUSTERS_CXX__
#define __MINNCLUSTERS_CXX__

#include "MinNClusters.h"

namespace larcv{

  void MinNClusters::_Configure_(const ::fcllite::PSet &pset)
  {
    _NClusters = pset.get<int> ("MinNClusters");
  }

  Cluster2DArray_t MinNClusters::_Process_(const larcv::Cluster2DArray_t& clusters,
					   const ::cv::Mat& img,
					   larcv::ImageMeta& meta)
  {

    if ( clusters.size() < _NClusters )
      return Cluster2DArray_t ();

    return clusters;
    
  }

}
#endif
