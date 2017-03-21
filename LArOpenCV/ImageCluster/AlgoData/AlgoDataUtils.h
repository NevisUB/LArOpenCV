#ifndef __ALGODATAUTILS_H__
#define __ALGODATAUTILS_H__

#include "ParticleCluster.h"
#include "TrackClusterCompound.h"
#include "Vertex.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataManager.h"

namespace larocv {
  namespace data {

    struct VertexTrackInfoCollection {
      const Vertex3D* vtx3d;
      std::vector<const ParticleCluster*> super_contour_v;
      std::vector<std::vector<const ParticleCluster*> >      particle_vv;
      std::vector<std::vector<const TrackClusterCompound*> > compound_vv;
      /*
      VertexTrackInfoCollection(const VertexTrackInfoCollection& obj)
      { vtx3d = obj.vtx3d;
	super_contour_v.resize(obj.super_contour_v.size());
	for(size_t i=0; i<super_contour_v.size(); ++i) super_contour_v[i] = obj.super_contour_v[i];
	particle_vv.resize(obj.particle_vv.size());
	for(size_t i=0; i<particle_vv.size(); ++i) {
	  particle_vv[i].resize(obj.particle_vv[i].size());
	  for(size_t j=0; j<particle_vv[i].size(); ++j)
	    particle_vv[i][j] = obj.particle_vv[i][j];
	}
	for(size_t i=0; i<compound_vv.size(); ++i) {
	  compound_vv[i].resize(obj.compound_vv[i].size());
	  for(size_t j=0; j<compound_vv[i].size(); ++j)
	    compound_vv[i][j] = obj.compound_vv[i][j];
	}
      }
      */
    };

    std::vector<VertexTrackInfoCollection>
    OrganizeVertexInfo(const AlgoDataAssManager& ass_man,
		       const Vertex3DArray& vtx3d_v,
		       std::vector<const ParticleClusterArray*> super_cluster_v = std::vector<const ParticleClusterArray*>(),
		       std::vector<const ParticleClusterArray*> part_cluster_v  = std::vector<const ParticleClusterArray*>(),
		       std::vector<const TrackClusterCompoundArray*> compound_v = std::vector<const TrackClusterCompoundArray*>()
		       );

    double
    Distance(const Vertex3D& vtx1, const Vertex3D& vtx2);

    bool
    Equals(const Vertex3D& vtx1, const Vertex3D& vtx2);
    
  }
}

#endif
