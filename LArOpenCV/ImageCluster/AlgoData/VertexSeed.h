#ifndef __VERTEXSEED_H__
#define __VERTEXSEED_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"

namespace larocv {
  namespace data {

    // store per compound
    class VertexSeed2D : public AlgoDataArrayElementBase {
    public:
      VertexSeed2D() { _Clear_(); }
      ~VertexSeed2D(){}

      void _Clear_() { _seed_v.clear(); }
      const std::vector<geo2d::Vector<float> >& seeds() const { return _seed_v; }

      void store_seed(const geo2d::Vector<float>& seed);
      void move_seed(geo2d::Vector<float>&& seed);

      void store_seeds(const std::vector<geo2d::Vector<float> >& seed_v);
      void move_seeds(std::vector<geo2d::Vector<float> >&& seed_v);
      
    private:
      std::vector<geo2d::Vector<float> > _seed_v;
      
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
