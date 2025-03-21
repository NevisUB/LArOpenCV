#ifndef __ALGODATA_TRACKPARTICLECLUSTERDATA_H__
#define __ALGODATA_TRACKPARTICLECLUSTERDATA_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"

namespace larocv {
  namespace data {
    
    class TrackParticleClusterData : public larocv::data::AlgoDataBase {
    public:
      TrackParticleClusterData()
      { Clear();}
      ~TrackParticleClusterData(){}
      /// attribute clear
      void Clear() {}

    private:

    };
  }
}
#endif
/** @} */ // end of doxygen group
