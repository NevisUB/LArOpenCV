#ifndef __ALGODATA_SPACEPT_H__
#define __ALGODATA_SPACEPT_H__

#include "Vertex.h"

namespace larocv {
  namespace data {
    
    class SpacePt {

    public :
      SpacePt();
      SpacePt(const Vertex3D& pt_, float q_);
      SpacePt(Vertex3D&& pt_, float q_);
      ~SpacePt();

      Vertex3D pt;
      float q;
      
    };
    
  }
}
#endif
