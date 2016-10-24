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

#include "Core/Line.h"
#include "Core/LineSegment.h"

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

    AlgorithmID_t _input_id;
    
    std::vector<GEO2D_ContourArray_t> _atomic_ctor_v_v;
    std::vector<std::vector<size_t> > _atomic_ctor_ass_v_v;

    std::vector<std::vector<geo2d::LineSegment<float>> >  _hull_line_v_v;
    std::vector<std::vector<geo2d::Line<float> > > _split_line_v_v;

    std::vector<unsigned> _n_original_clusters_v;
    
    void set_data(unsigned n_original_clusters,
		  const GEO2D_ContourArray_t& atomic_ctor_v,
		  const std::vector<size_t>& atomic_ctor_ass_v,
		  short plane);
    
  };
}

#endif
/** @} */ // end of doxygen group 

