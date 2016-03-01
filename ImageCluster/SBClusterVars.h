/**
 * \file SBClusterVars.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class SBClusterVars
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef SBCLUSTERVARS_H
#define SBCLUSTERVARS_H

#include "AlgoVarsBase.h"

namespace larcv {
  /**
     \class SBClusterVars
     User defined class SBClusterVars ... these comments are used to generate
     doxygen documentation!
  */
  class SBClusterVars : public AlgoVarsBase {
    
  public:
    
    /// Default constructor
    SBClusterVars(){}
    
    /// Default destructor
    ~SBClusterVars(){}

    /// Reset function to be called per process
    void Reset() {}
    
  };
}

#endif
/** @} */ // end of doxygen group 

