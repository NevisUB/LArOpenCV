#ifndef __ALGODATA_CLUSTER_CXX__
#define __ALGODATA_CLUSTER_CXX__

#include "AlgoDataCluster.h"

namespace larocv {
  namespace data {

    /// sets the end point
    void ClusterCompound::set_end_pt(geo2d::Vector<float>& ep) {
      _end_pt = ep;
    }
    /// queries the end point
    const geo2d::Vector<float>& ClusterCompound::end_pt() const {
      return _end_pt;
    }
    
  }
}
#endif
