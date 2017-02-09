#ifndef __VERTEXSEED_H__
#define __VERTEXSEED_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"

namespace larocv {
  namespace data {

    // store per compound
    class VertexSeed2D : public AlgoDataArrayElementBase,
			 public geo2d::Vector<float>
    {
    public:
      VertexSeed2D() { _Clear_(); }
      VertexSeed2D(const geo2d::Vector<float>& pt) { this->x=pt.x; this->y=pt.y; }
      //VertexSeed2D(geo2d::Vector<float>&& pt) { (*this)=std::move(pt); }
      ~VertexSeed2D(){}

      void _Clear_() { this->x=kINVALID_FLOAT; this->y=kINVALID_FLOAT; }
      
    private:
      
    };
    
    // store per compound
    class VertexSeed3D : public AlgoDataArrayElementBase {
    public:
      VertexSeed3D() { _Clear_(); }
      ~VertexSeed3D(){}
      
      void _Clear_() {}
    private:      
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
