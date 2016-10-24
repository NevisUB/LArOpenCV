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
#ifndef PCACANDIDATESDATA_H
#define PCACANDIDATESDATA_H

#include "AlgoDataBase.h"
#include "Core/Line.h"

namespace larocv {
  /**
     \class PCACandidatesData
     @brief only for vic
  */
  class PCACandidatesData : public AlgoDataBase {
    
  public:
    
    /// Default constructor
    PCACandidatesData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    {  Clear(); }
    
    /// Default destructor
    ~PCACandidatesData(){}

    /// Clear method override
    void Clear();

    AlgorithmID_t _input_id;
    std::vector<std::vector<std::vector<geo2d::Line<float> > > >   _ctor_lines_v_v_v;
    std::vector<std::vector<std::vector<geo2d::Vector<float> > > > _ipoints_v_v_v;
    std::vector<std::vector<std::vector<size_t > > > _atomic_id_v_v_v;
    void set_data(const std::vector<geo2d::Line<float> >& ctor_lines_v,
		  const std::vector<geo2d::Vector<float> >& ipoints_v,
		  const std::vector<size_t>& atomic_id_v,
		  short plane);
    
  };
}

#endif
/** @} */ // end of doxygen group 


