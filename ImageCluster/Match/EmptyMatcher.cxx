#ifndef __EMPTYMATCHER_CXX__
#define __EMPTYMATCHER_CXX__

#include "EmptyMatcher.h"

namespace larocv{

  void EmptyMatcher::_Configure_(const ::fcllite::PSet &pset)
  {}

  double EmptyMatcher::_Process_(const larocv::Cluster2DPtrArray_t& clusters, const std::vector<double>& vtx)
  { return -1.; }

}
#endif
