#ifndef __PARTICLE_CLUSTER_H__
#define __PARTICLE_CLUSTER_H__

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"

namespace larocv {
  namespace data {
    /**
       \class ParticleCluster
       @brief A cluster associated with a vertex and hence represents a particle
    */
    class ParticleCluster : public AlgoDataArrayElementBase {
    public:
      ParticleCluster(){ _Clear_(); }
      //ParticleCluster(const GEO2D_Contour_t& ctor) : _ctor(ctor) {}
      // ParticleCluster(GEO2D_Contour_t&& ctor) : _ctor(std::move(ctor)) {}
      ~ParticleCluster() {}
      
      void _Clear_() { _ctor.clear(); }
      GEO2D_Contour_t _ctor; ///< contour to define a cluster
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
