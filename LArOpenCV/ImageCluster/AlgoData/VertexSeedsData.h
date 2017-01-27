#ifndef __ALGODATA_VERTEXSEEDSDATA_H__
#define __ALGODATA_VERTEXSEEDSDATA_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/AlgoData/DefectClusterData.h"

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

      std::vector<ClusterCompound> _cluscomp_v;
      std::vector<geo2d::Vector<float> > _pca_v;
      
    };
  }
}
#endif
