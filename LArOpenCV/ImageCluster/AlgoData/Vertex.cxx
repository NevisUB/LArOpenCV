#ifndef __ALGODATA_VERTEX_CXX__
#define __ALGODATA_VERTEX_CXX__

#include "Vertex.h"

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
      weight = -1;
      dtheta_xs = kINVALID_FLOAT;
    }
    
    float CircleVertex::sum_dtheta() const
    { float res=0; for(auto const& d : dtheta_v) res += d; return res; }
    
    float CircleVertex::mean_dtheta() const
    { float res=0; for(auto const& d : dtheta_v) res += d; return (res / (float) dtheta_v.size()) ; }

    double Vertex3D::dist(const Vertex3D& pt) const
    { return sqrt(pow(x-pt.x,2)+pow(y-pt.y,2)+pow(z-pt.z,2)); }
    
  }
}
#endif
