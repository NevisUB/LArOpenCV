#ifndef __PARTICLE_CLUSTER_H__
#define __PARTICLE_CLUSTER_H__

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
/*
  @brief: ParticleCluster holds a contour and may represent a unique particle
*/
namespace larocv {
  namespace data {
    /**
       \class ParticleCluster
       @brief A cluster associated with a vertex and hence represents a particle
    */
    
    enum class ParticleType_t : unsigned
    { kUnknown=0, kTrack, kShower };
    
    class ParticleCluster : public AlgoDataArrayElementBase {
    public:
      ParticleCluster() : AlgoDataArrayElementBase()
      { Clear(); }
      ~ParticleCluster() {}
      
      void _Clear_() { _ctor.clear(); type=ParticleType_t::kUnknown; }
      
      void Clear();
      
      GEO2D_Contour_t _ctor; ///< contour to define a cluster
      ParticleType_t type;
      double _angle;

      std::vector<float> _vertex_dqds;
      
    private:
    };
    
    /**
       \class ParticleClusterArray
    */
    typedef AlgoDataArrayTemplate<ParticleCluster> ParticleClusterArray;
 
  }
}
#endif
/** @} */ // end of doxygen group
