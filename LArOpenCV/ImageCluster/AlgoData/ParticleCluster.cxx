#ifndef __PARTICLE_CLUSTER_CXX__
#define __PARTICLE_CLUSTER_CXX__

#include "ParticleCluster.h"

namespace larocv {
  namespace data {

    void ParticleCluster::_Clear_()
    { 
      type=ParticleCluster::ParticleType_t::kUnknown;
      _ctor.clear();
      _vertex_dqds.clear();
      _truncated_dqds.clear();
      _angle_scan_end_point.x = -9999;
      _angle_scan_end_point.y = -9999;
      _atom_end_point.x = -9999;
      _atom_end_point.y = -9999;
      _atom.clear();
      _supercluster_v.clear();
    }

    void Particle::_Clear_()
    {
      type = ParticleCluster::ParticleType_t::kUnknown; 
      _par_v.clear(); 
      _par_v.resize(3);
    }


  }
}
#endif
