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
#include "Core/Vector.h"

#include "Core/Line.h"

#include <iostream>

typedef std::vector<std::vector<geo2d::Vector<int> > > GEO2D_ContourArray_t;
typedef std::vector<geo2d::Vector<int> >               GEO2D_Contour_t;

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

    std::vector<std::vector<geo2d::Line<float> > > _ctor_lines_v_v;
    std::vector<std::vector<geo2d::Vector<float> > > _ipoints_v_v;

    void set_data(const std::vector<geo2d::Line<float> >& ctor_lines_v,
		  const std::vector<geo2d::Vector<float> >& ipoints_v,
		  short plane);
    
  };
}

#endif
/** @} */ // end of doxygen group 


