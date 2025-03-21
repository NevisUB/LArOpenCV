#ifndef __VERTEXSEED_H__
#define __VERTEXSEED_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
/*
  @brief: storage for 2D/3D vertex seeds
*/
namespace larocv {
  namespace data {

    enum class SeedType_t : unsigned {
      kUnknown,
	kDefect,
	kPCA,
	kEdge,
	kEnd,
	kTrack
    };
    
    // store per compound
    class VertexSeed2D : public AlgoDataArrayElementBase,
			 public geo2d::Vector<float>
    {
    public:
      VertexSeed2D() { Clear(); }
      VertexSeed2D(const geo2d::Vector<float>& pt)
      { this->x=pt.x; this->y=pt.y; radius=kINVALID_FLOAT; score=kINVALID_FLOAT;}
      VertexSeed2D(const geo2d::Vector<float>& pt,float rad)
      { this->x=pt.x; this->y=pt.y; radius=rad; score=kINVALID_FLOAT;}
      VertexSeed2D(const geo2d::Circle<float>& circle)
      { this->x=circle.center.x; this->y=circle.center.y; radius=circle.radius; score=kINVALID_FLOAT;}
      VertexSeed2D(const Vertex2D& vtx2d)
      { this->x = vtx2d.pt.x; this->y = vtx2d.pt.y; radius=kINVALID_FLOAT; score=vtx2d.score; }
      
      ~VertexSeed2D(){}
      
      SeedType_t type;
      float radius;
      float score;
      
    protected:

      void _Clear_() {
	this->x=kINVALID_FLOAT;
	this->y=kINVALID_FLOAT;
	type=SeedType_t::kUnknown;
	radius=kINVALID_FLOAT;
	score=kINVALID_FLOAT;
      }
      
      geo2d::Circle<float> as_circle()
      { return geo2d::Circle<float>(this->x,this->y,radius); }
      
    };
    
    // store per compound
    class VertexSeed3D : public AlgoDataArrayElementBase {
    public:
      VertexSeed3D() { Clear(); }
      VertexSeed3D(const Vertex3D& vtx3d);
      VertexSeed3D(const VertexSeed3D& vtxseed3d);
      ~VertexSeed3D(){}
      
      /// Plane-wise 2D vertex point
      std::vector<VertexSeed2D> vtx2d_v;
      /// 3D vertex location
      double x, y, z;
      SeedType_t type;
      
    protected:
      /// attribute clear method
      void _Clear_()
      { vtx2d_v.clear(); x = y = z = kINVALID_DOUBLE; type=SeedType_t::kUnknown; }
      
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
