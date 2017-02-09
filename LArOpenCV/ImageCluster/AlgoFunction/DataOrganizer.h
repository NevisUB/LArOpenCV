#ifndef __DATAORGANIZER_H__
#define __DATAORGANIZER_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataManager.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"

namespace larocv {
  namespace data {

    std::vector<VertexTrackInfoCollection>
    OrganizeVertexInfo(const AlgoDataAssManager& ass_man,
		       const Vertex3DArray& vtx3d_v,
		       std::vector<const ParticleClusterArray*>& super_cluster_v = std::vector<const ParticleClusterArray*>(),
		       std::vector<const ParticleClusterArray*>& part_cluster_v  = std::vector<const ParticleClusterArray*>(),
		       std::vector<const TrackClusterCompoundArray*>& compound_v = std::vector<const TrackClusterCompoundArray*>()
		       );
    
  }
}
#endif
