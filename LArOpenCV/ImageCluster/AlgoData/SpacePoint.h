#ifndef __ALGODATA_SPACEPOINT_H__
#define __ALGODATA_SPACEPOINT_H__

#include "Vertex.h"

namespace larocv {
  namespace data {
    
    class SpacePoint {

    public :
      SpacePoint();
      SpacePoint(const Vertex3D& pt_, float q_);
      SpacePoint(Vertex3D&& pt_, float q_);
      ~SpacePoint();

      Vertex3D pt;
      float q;
      
    };
    
  }
}
#endif
