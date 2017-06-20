#ifndef __ALGODATA_SIMPLECOLINEARDATA_H__
#define __ALGODATA_SIMPLECOLINEARDATA_H__

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"


namespace larocv {

  namespace data {

    /**
       \class SimpleColinearData
       @brief only for vic
    */
    
    class SimpleColinearData : public AlgoDataBase {
      
    public:
    
      /// Default constructor
      SimpleColinearData()
      {  Clear(); }    
      /// Default destructor
      ~SimpleColinearData(){}
      
      void Clear();

    private:
      
    };
    
  }
}
#endif
/** @} */ // end of doxygen group