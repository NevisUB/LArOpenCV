/**
 * \file HIPClusterData.h
 *
 * \ingroup Base
 * 
 * \brief Class def header for a class HIPClusterData
 *
 * @author kazuhiro
 */

/** \addtogroup Base

    @{*/
#ifndef CIRCLEVERTEXDATA_H
#define CIRCLEVERTEXDATA_H

#include "AlgoDataBase.h"
#include "Core/Line.h"

namespace larocv {
  /**
     \class CircleVertexData
     @brief only for vic
  */
  class CircleVertexData : public AlgoDataBase {
    
  public:
    
    /// Default constructor
    CircleVertexData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    {  Clear(); }
    
    /// Default destructor
    ~CircleVertexData(){}

    /// Clear method override
    void Clear();
    void set_data();
    
  };
}

#endif
/** @} */ // end of doxygen group 


