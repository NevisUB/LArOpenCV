/**
 * \file PCACandidatesData.h
 *
 * \ingroup AlgoData
 * 
 * \brief Class def header for a class PCACandidatesData
 *
 * @author vic
 */

/** \addtogroup AlgoData

    @{*/

#ifndef __ALGODATA_EDGEVERTEXCANDIDATESDATA_H__
#define __ALGODATA_EDGEVERTEXCANDIDATESDATA_H__

#include "Core/Line.h"
#include "Core/LArOCVTypes.h"
#include "Base/AlgoDataBase.h"
namespace larocv {

  namespace data {


    
    /**
       \class EdgeVertexCandidatesData
       @brief only for vic
    */
    class EdgeVertexCandidatesData : public AlgoDataBase {
      
    public:
    
      /// Default constructor
      EdgeVertexCandidatesData(std::string name="NoName", AlgorithmID_t id=0)
	: AlgoDataBase(name,id)
      {   }    
      /// Default destructor
      ~EdgeVertexCandidatesData(){}
      
      void Clear() { }
      
      
      
      
    private:
      
    };
  }  
}
#endif
/** @} */ // end of doxygen group
