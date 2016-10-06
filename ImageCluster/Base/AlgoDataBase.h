/**
 * \file AlgoDataBase.h
 *
 * \ingroup Base
 * 
 * \brief Class def header for a class AlgoDataBase
 *
 * @author kazuhiro
 */

/** \addtogroup Base

    @{*/
#ifndef ALGODATABASE_H
#define ALGODATABASE_H

#include "ImageClusterTypes.h"

namespace larocv {
  /**
     \class AlgoDataBase
     @brief Base class for algorithm-specific data representation 
  */
  class AlgoDataBase{
    
  public:
    
    /// Default constructor
    AlgoDataBase(std::string name="NoName", AlgorithmID_t id=0)
      : _name(name)
      , _id(id)
    {}
    
    /// Default destructor
    virtual ~AlgoDataBase(){}

    /// Clear method to be implemented
    virtual void Clear() = 0;

    /// Name to identify associated algorithm
    const std::string& Name() const;

    /// AlgorithmID_t to identify associated algorithm
    AlgorithmID_t ID() const;

  private:

    std::string   _name; ///< Associated algorithm's name
    AlgorithmID_t _id;   ///< Associated algorithm's id
    
  };

}

#endif
/** @} */ // end of doxygen group 

