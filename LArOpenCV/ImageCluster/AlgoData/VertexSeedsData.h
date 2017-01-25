#ifndef __ALGODATA_VERTEXSEEDSDATA_H__
#define __ALGODATA_VERTEXSEEDSDATA_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"

namespace larocv {

  namespace data {

    /*
      \class VertexSeedsData
    */
    class VertexSeedsData : public larocv::data::AlgoDataBase {
    public:
      VertexSeedsData()
      { Clear();}
      ~VertexSeedsData(){}
      
      void Clear() {}
      
    };
  }
}
#endif
