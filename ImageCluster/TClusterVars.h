/**
 * \file TClusterVars.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class TClusterVars
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef TCLUSTERVARS_H
#define TCLUSTERVARS_H

#include "AlgoVarsBase.h"

namespace larcv {
  /**
     \class TClusterVars
     User defined class TClusterVars ... these comments are used to generate
     doxygen documentation!
  */
  class TClusterVars : public AlgoVarsBase {
    
  public:
    
    /// Default constructor
    TClusterVars(){}
    
    /// Default destructor
    ~TClusterVars(){}

    /// Reset function to be called per process
    void Reset() {}
    
  };
}

#endif
/** @} */ // end of doxygen group 

