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
#ifndef DEFECTCLUSTERDATA_H
#define DEFECTCLUSTERDATA_H

#include "AlgoDataBase.h"
#include "Core/Vector.h"

#include <iostream>

typedef std::vector<std::vector<geo2d::Vector<int> > > GEO2D_ContourArray_t;
typedef std::vector<geo2d::Vector<int> >               GEO2D_Contour_t;

namespace larocv {
  /**
     \class DefectClusterData
     @brief only for vic
  */
  class DefectClusterData : public AlgoDataBase {
    
  public:
    
    /// Default constructor
    DefectClusterData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    {  Clear(); }
    
    /// Default destructor
    ~DefectClusterData(){}

    /// Clear method override
    void Clear();
    std::vector<GEO2D_ContourArray_t> _atomic_ctor_v_v;

    void set_data(const GEO2D_ContourArray_t& atomic_ctor_v,short plane);
    
  };
}

#endif
/** @} */ // end of doxygen group 

