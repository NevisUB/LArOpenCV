/**
 * \file AlgoVarsBase.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class AlgoVarsBase
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef ALGOVARSBASE_H
#define ALGOVARSBASE_H

namespace larcv {
  /**
     \class AlgoVarsBase
     User defined class AlgoVarsBase ... these comments are used to generate
     doxygen documentation!
  */
  class AlgoVarsBase{
    
  public:
    
    /// Default constructor
    AlgoVarsBase(){}
    
    /// Default destructor
    virtual ~AlgoVarsBase(){}

    /// Reset function to be called per process
    virtual void Reset() = 0;
    
  };
}

#endif
/** @} */ // end of doxygen group 

