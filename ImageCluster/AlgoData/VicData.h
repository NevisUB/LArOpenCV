/**
 * \file VicData.h
 *
 * \ingroup Base
 * 
 * \brief Class def header for a class VicData
 *
 * @author kazuhiro
 */

/** \addtogroup Base

    @{*/
#ifndef VICDATA_H
#define VICDATA_H

#include "AlgoDataBase.h"
#include <iostream>
namespace larocv {
  /**
     \class VicData
     @brief only for vic
  */
  class VicData : public AlgoDataBase {
    
  public:
    
    /// Default constructor
    VicData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    {std::cout << "\n\n\n\n\nVicData constructed\n\n\n\n\n";}
    
    /// Default destructor
    ~VicData(){}

    /// Clear method override
    void Clear();

    double num_clusters;
  };
}

#endif
/** @} */ // end of doxygen group 

