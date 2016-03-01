/**
 * \file PCAProjection.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PCAProjection
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef PCAPROJECTIONVARS_H
#define PCAPROJECTIONVARS_H

#include "AlgoVarsBase.h"

namespace larcv {
  /**
     \class PCAProjection
     User defined class PCAProjection ... these comments are used to generate
     doxygen documentation!
  */
  class PCAProjectionVars : public AlgoVarsBase {
    
  public:
    
    /// Default constructor
    PCAProjectionVars(){}
    
    /// Default destructor
    ~PCAProjectionVars(){}

    /// Reset function to be called per process
    void Reset() {}
    
  };
}

#endif
/** @} */ // end of doxygen group 

