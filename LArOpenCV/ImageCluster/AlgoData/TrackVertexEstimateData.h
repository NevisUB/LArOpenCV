#ifndef __ALGODATA_TRACKVERTEXESTIMATE_H__
#define __ALGODATA_TRACKVERTEXESTIMATE_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"

namespace larocv {

  namespace data {

    /*
      \class TrackVertexEstimateData
    */
    class TrackVertexEstimateData : public larocv::data::AlgoDataBase {
    public:
      TrackVertexEstimateData()
      { Clear();}
      ~TrackVertexEstimateData(){}
      
      void Clear() {}

    };
  }
}
#endif
