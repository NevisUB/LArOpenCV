#ifndef __ALGODATA_SIMPLECOLINEARDATA_H__
#define __ALGODATA_SIMPLECOLINEARDATA_H__

#include "Core/LArOCVTypes.h"
#include "Base/AlgoDataBase.h"


namespace larocv {

  namespace data {

    /**
       \class SimpleColinearData
       @brief only for vic
    */
    
    class SimpleColinearData : public AlgoDataBase {
      
    public:
    
      /// Default constructor
      SimpleColinearData(std::string name="NoName", AlgorithmID_t id=0)
	: AlgoDataBase(name,id)
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
