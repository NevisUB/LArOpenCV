#ifndef __ALGODATA_SPACEPOINT_CXX__
#define __ALGODATA_SPACEPOINT_CXX__

#include "SpacePoint.h"

namespace larocv {
  namespace data {
    SpacePoint::SpacePoint() {}
    SpacePoint::SpacePoint(const Vertex3D& pt_, float q_) : pt(pt_), q(q_){}
    SpacePoint::SpacePoint(Vertex3D&& pt_, float q_) :  pt(std::move(pt_)), q(q_) {}
    SpacePoint::~SpacePoint() {}
  }
}
#endif
