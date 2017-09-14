#ifndef __ALGODATAUTILS_H__
#define __ALGODATAUTILS_H__

#include "ParticleCluster.h"
#include "TrackClusterCompound.h"
#include "Vertex.h"
#include "VertexSeed.h"
#include "SpacePt.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataManager.h"

#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#endif
#endif

namespace larocv {
  namespace data {

    struct VertexTrackInfoCollection {
      const Vertex3D* vtx3d;
      std::vector<const ParticleCluster*> super_contour_v;
      std::vector<std::vector<const ParticleCluster*> >      particle_vv;
      std::vector<std::vector<const TrackClusterCompound*> > compound_vv;
    };

    std::vector<VertexTrackInfoCollection>
    OrganizeVertexInfo(const AlgoDataAssManager& ass_man,
		       const Vertex3DArray& vtx3d_v,
		       std::vector<const ParticleClusterArray*> super_cluster_v = std::vector<const ParticleClusterArray*>(),
		       std::vector<const ParticleClusterArray*> part_cluster_v  = std::vector<const ParticleClusterArray*>(),
		       std::vector<const TrackClusterCompoundArray*> compound_v = std::vector<const TrackClusterCompoundArray*>());

    
    Vertex3D Difference(const Vertex3D& vtx1, const Vertex3D& vtx2);

    double Distance(const Vertex3D& vtx1, const Vertex3D& vtx2);    
    double Distance(const SpacePt& vtx1, const Vertex3D& vtx2);
    double Distance(const Vertex3D& vtx1, const SpacePt& vtx2);

    bool Equals(const Vertex3D& vtx1, const Vertex3D& vtx2);

    std::vector<Vertex2D> 
      Seed2Vertex(const std::vector<VertexSeed2D>& svtx2d_v);

    Vertex2D Seed2Vertex(const VertexSeed2D& svtx2d);
    Vertex3D Seed2Vertex(const VertexSeed3D& svtx2d);

    std::pair<float,float> 
      Angle3D(const std::vector<data::SpacePt>& sps_v,
	      const data::Vertex3D& start3d);

    std::pair<float,float> 
      Angle3D(const std::vector<data::SpacePt>& sps_v,
	      const data::Vertex3D& start3d,
	      data::Vertex3D& mean_pt);

    std::pair<float,float> 
      Angle3D(const data::Vertex3D& vtx1,
	      const data::Vertex3D& vtx2);

  }
}

#endif
