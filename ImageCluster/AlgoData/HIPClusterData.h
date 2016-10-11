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
#ifndef HIPCLUSTERDATA_H
#define HIPCLUSTERDATA_H

#include "AlgoDataBase.h"
#include "Core/Vector.h"

#include <iostream>

typedef std::vector<std::vector<geo2d::Vector<int> > > GEO2D_ContourArray_t;
typedef std::vector<geo2d::Vector<int> >               GEO2D_Contour_t;

namespace larocv {
  /**
     \class HIPClusterData
     @brief only for vic
  */
  class HIPClusterData : public AlgoDataBase {
    
  public:
    
    /// Default constructor
    HIPClusterData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear(); }
    
    /// Default destructor
    ~HIPClusterData(){}

    /// Clear method override
    void Clear();

    std::vector<GEO2D_ContourArray_t> _mip_ctors_v_v;
    std::vector<GEO2D_ContourArray_t> _hip_ctors_v_v;
    std::vector<GEO2D_ContourArray_t> _all_ctors_v_v;

    void set_data(const GEO2D_ContourArray_t& mips,
		  const GEO2D_ContourArray_t& hips,
		  const GEO2D_ContourArray_t& alls,
		  short plane);
    
  };
}

#endif
/** @} */ // end of doxygen group 

