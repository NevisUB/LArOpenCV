#ifndef __EMPTYMATCHER_CXX__
#define __EMPTYMATCHER_CXX__

#include "EmptyMatcher.h"

namespace larcv{

  void EmptyMatcher::_Configure_(const ::fcllite::PSet &pset)
  {}

  double EmptyMatcher::_Process_(const larcv::Cluster2DPtrArray_t& clusters)
  { return -1.; }

}
#endif
