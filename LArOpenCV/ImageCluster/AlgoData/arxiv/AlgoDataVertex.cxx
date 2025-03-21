#ifndef __ALGODATA_VERTEX_CXX__
#define __ALGODATA_VERTEX_CXX__

#include "AlgoDataVertex.h"

namespace larocv {
  namespace data {
    //////////////////////////////////////////////////////////////
    
    void CircleVertex::Clear()
    {
      center.x = center.y = -1;
      error.x  = error.y  = -1;
      radius   = -1;
      xs_v.clear();
      dtheta_v.clear();
    }
    
    float CircleVertex::sum_dtheta() const
    { float res=0; for(auto const& d : dtheta_v) res += d; return res; }
  }
}
#endif
