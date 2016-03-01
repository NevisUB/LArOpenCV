/**
 * \file TrackShowerVars.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class TrackShowerVars
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef TRACKSHOWERVARS_H
#define TRACKSHOWERVARS_H

#include "AlgoVarsBase.h"

namespace larcv {
  /**
     \class TrackShowerVars
     User defined class TrackShowerVars ... these comments are used to generate
     doxygen documentation!
  */
  class TrackShowerVars : public AlgoVarsBase {
    
  public:
    
    /// Default constructor
    TrackShowerVars(){}
    
    /// Default destructor
    ~TrackShowerVars(){}

    /// Reset function to be called per process
    void Reset() {}
    
  };
}

#endif
/** @} */ // end of doxygen group 

