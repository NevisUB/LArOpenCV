#ifndef __ALGOFUNCTIONTYPES_H__
#define __ALGOFUNCTIONTYPES_H__

#include "ParticleCluster.h"
#include "TrackClusterCompound.h"
#include "Vertex.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataManager.h"

namespace larocv {
  namespace data {

    struct VertexTrackInfoCollection {
      const Vertex3D* vtx3d;
      std::vector<const ParticleCluster*> super_contour_v;
      std::vector<std::vector<const ParticleCluster*> >      particle_v;
      std::vector<std::vector<const TrackClusterCompound*> > compound_v;
    };

    std::vector<VertexTrackInfoCollection>
    OrganizeVertexInfo(const AlgoDataAssManager& ass_man,
		       const Vertex3DArray& vtx3d_v,
		       std::vector<const ParticleClusterArray*> super_cluster_v = std::vector<const ParticleClusterArray*>(),
		       std::vector<const ParticleClusterArray*> part_cluster_v  = std::vector<const ParticleClusterArray*>(),
		       std::vector<const TrackClusterCompoundArray*> compound_v = std::vector<const TrackClusterCompoundArray*>()
		       );

  }
}

#endif
