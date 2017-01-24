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
  namespace data {
    /**
       \class AlgoDataBase
       @brief Base class for algorithm-specific data representation 
    */
    class AlgoDataBase{
      
    public:
      
      /// Default constructor
      AlgoDataBase()
      {}
      
      /// Default destructor
      virtual ~AlgoDataBase(){}
      
      /// Clear method to be implemented
      virtual void Clear() = 0;
      
    private:
      
    };
  }
}

#endif
/** @} */ // end of doxygen group 

