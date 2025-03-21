#ifndef __ALGODATA_SPACEPT_CXX__
#define __ALGODATA_SPACEPT_CXX__

#include "SpacePt.h"

namespace larocv {
  namespace data {
    SpacePt::SpacePt() {}
    SpacePt::SpacePt(const Vertex3D& pt_, float q_) : pt(pt_), q(q_){}
    SpacePt::SpacePt(Vertex3D&& pt_, float q_) :  pt(std::move(pt_)), q(q_) {}
    SpacePt::~SpacePt() {}
  }
}
#endif
