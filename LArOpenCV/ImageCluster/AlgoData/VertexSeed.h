#ifndef __VERTEXSEED_H__
#define __VERTEXSEED_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
namespace larocv {
  namespace data {

    // store per compound
    class VertexSeed2D : public AlgoDataArrayElementBase,
			 public geo2d::Vector<float>
    {
    public:
      VertexSeed2D() { Clear(); }
      VertexSeed2D(const geo2d::Vector<float>& pt) { this->x=pt.x; this->y=pt.y; }
      //VertexSeed2D(geo2d::Vector<float>&& pt) { (*this)=std::move(pt); }
      ~VertexSeed2D(){}
    protected:
      void _Clear_() { this->x=kINVALID_FLOAT; this->y=kINVALID_FLOAT; }
    };
    
    // store per compound
    class VertexSeed3D : public AlgoDataArrayElementBase {
    public:
      VertexSeed3D() { Clear(); }
      VertexSeed3D(const Vertex3D& vtx3d)
      { vtx2d_v = vtx3d.vtx2d_v; x = vtx3d.x; y = vtx3d.y; z = vtx3d.z; }
      ~VertexSeed3D(){}
      
      /// Plane-wise 2D vertex point
      std::vector<larocv::data::Vertex2D> vtx2d_v;
      /// 3D vertex location
      double x, y, z;
    protected:
      /// attribute clear method
      void _Clear_()
      { vtx2d_v.clear(); x = y = z = kINVALID_DOUBLE; }
    };
    
    /**
       \class VertexSeed2DArray
    */
    typedef AlgoDataArrayTemplate<VertexSeed2D> VertexSeed2DArray;
    /**
       \class VertexSeed3DArray
    */
    typedef AlgoDataArrayTemplate<VertexSeed3D> VertexSeed3DArray;


  }
}
#endif
