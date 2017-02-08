#ifndef __ALGODATA_CONTOURARRAYDATA_H__
#define __ALGODATA_CONTOURARRAYDATA_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "AlgoDataVertex.h"
#include "LArOpenCV/Core/larbys.h"
namespace larocv {

  namespace data {

    /*
      \class ContourArrayData
    */
    class ContourArrayData : public larocv::data::AlgoDataBase,
			     public GEO2D_ContourArray_t
    {
    public:
      ContourArrayData();
      ~ContourArrayData(){}
      
      void Clear();
    };
    
  }
}
#endif
