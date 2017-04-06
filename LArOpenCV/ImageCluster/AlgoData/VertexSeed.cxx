#ifndef __VERTEXSEED_CXX__
#define __VERTEXSEED_CXX__

#include "VertexSeed.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {

  namespace data {

    VertexSeed3D::VertexSeed3D(const Vertex3D& vtx3d) {

      for(const auto& vtx2d : vtx3d.vtx2d_v)
	vtx2d_v.emplace_back(vtx2d);
      
      x = vtx3d.x;
      y = vtx3d.y;
      z = vtx3d.z;
    }
    
    VertexSeed3D::VertexSeed3D(const VertexSeed3D& vtxseed3d) {
      vtx2d_v = vtxseed3d.vtx2d_v;
      x = vtxseed3d.x;
      y = vtxseed3d.y;
      z = vtxseed3d.z;
    }

    
  }
}
#endif
