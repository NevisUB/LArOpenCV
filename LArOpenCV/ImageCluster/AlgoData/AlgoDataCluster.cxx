#ifndef __ALGODATA_CLUSTER_CXX__
#define __ALGODATA_CLUSTER_CXX__

#include "AlgoDataCluster.h"
#include "LArOpenCV/Core/larbys.h"

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
    /// set the atomic ordering
    void ClusterCompound::set_atomic_order(std::vector<size_t> atom_idx_v) {
      _ordered_index_v = atom_idx_v;
    }

    /// retrieve a list of atomic clusters
    std::vector<larocv::data::AtomicContour*> ClusterCompound::get_ordered_atoms(){

      if (_ordered_index_v.empty())
	throw larbys("Atomic ordering not set");
      
      std::vector<larocv::data::AtomicContour*> ordered_atomics;
      ordered_atomics.resize(this->size());

      for(size_t idx=0;idx<ordered_atomics.size();++idx)
	ordered_atomics[idx] = & ( (*this)[ _ordered_index_v[idx] ] );
      
      return ordered_atomics;
      
    }
    
  }
}
#endif
