/**
 * \file SatelliteMergeVars.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class SatelliteMergeVars
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef SATELLITEMERGEVARS_H
#define SATELLITEMERGEVARS_H

#include "AlgoVarsBase.h"

namespace larcv {
  /**
     \class SatelliteMergeVars
     User defined class SatelliteMergeVars ... these comments are used to generate
     doxygen documentation!
  */
  class SatelliteMergeVars : public AlgoVarsBase {
    
  public:
    
    /// Default constructor
    SatelliteMergeVars(){}
    
    /// Default destructor
    ~SatelliteMergeVars(){}

    /// Reset function to be called per process
    void Reset() {}
    
  };
}

#endif
/** @} */ // end of doxygen group 

