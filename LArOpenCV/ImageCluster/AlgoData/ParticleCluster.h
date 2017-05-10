#ifndef __PARTICLE_CLUSTER_H__
#define __PARTICLE_CLUSTER_H__

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include <array>

/*
  @brief: ParticleCluster.h holds a contour and may represent a unique particle
*/
namespace larocv {
  namespace data {
    
    enum class ParticleType_t : unsigned
    { kUnknown=0, kTrack, kShower };
    
    /**
       \class ParticleCluster
       @brief A cluster associated with a vertex and hence represents a particle
    */
    
    class ParticleCluster : public AlgoDataArrayElementBase {
    public:
      ParticleCluster() : AlgoDataArrayElementBase()
      { Clear(); }
      ~ParticleCluster() {}
      
      void _Clear_() { _ctor.clear(); type=ParticleType_t::kUnknown; }
      GEO2D_Contour_t _ctor; ///< contour to define a cluster
      ParticleType_t type;
      double _angle;
      
    };

    /**
       \class Particle
       @brief A group of particle clusters associated together to make an interaction
    */
    class Particle : public AlgoDataArrayElementBase {
    public:
      Particle() : AlgoDataArrayElementBase()
      { Clear(); }
      ~Particle() {}

      void _Clear_() { type = ParticleType_t::kUnknown; _par_v.clear(); _par_v.resize(3);}

      ParticleType_t type;
      
      // Particle cluster per plane, can be empty
      std::vector<ParticleCluster> _par_v;

      // Score
      float score;
      
    };
    
    /**
       \class ParticleClusterArray
    */
    typedef AlgoDataArrayTemplate<Particle> ParticleArray;
    typedef AlgoDataArrayTemplate<ParticleCluster> ParticleClusterArray;
 
  }
}
#endif
/** @} */ // end of doxygen group
