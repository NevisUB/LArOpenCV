/**
 * \file PCASegmentationVars.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PCASegmentationVars
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef PCASEGMENTATIONVARS_H
#define PCASEGMENTATIONVARS_H

#include "AlgoVarsBase.h"

namespace larcv {
  /**
     \class PCASegmentationVars
     User defined class PCASegmentationVars ... these comments are used to generate
     doxygen documentation!
  */
  class PCASegmentationVars : public AlgoVarsBase {
    
  public:
    
    /// Default constructor
    PCASegmentationVars(){}
    
    /// Default destructor
    ~PCASegmentationVars(){}

    /// Reset function to be called per process
    void Reset() {}
    
  };
}

#endif
/** @} */ // end of doxygen group 

