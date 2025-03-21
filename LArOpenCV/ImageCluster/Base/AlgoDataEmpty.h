/**
 * \file AlgoDataEmpty.h
 *
 * \ingroup Base
 * 
 * \brief Class def header for a class AlgoDataEmpty
 *
 * @author kazuhiro
 */

/** \addtogroup Base

    @{*/
#ifndef ALGODATAEMPTY_H
#define ALGODATAEMPTY_H

#include "AlgoDataBase.h"

namespace larocv {
  namespace data {
    /**
       \class AlgoDataEmpty
       @brief only for vic
    */
    class AlgoDataEmpty : public AlgoDataBase {
      
    public:
      
      /// Default constructor
      AlgoDataEmpty(){}
      
      /// Default destructor
      ~AlgoDataEmpty(){}
      
      /// Clear method override
      void Clear() {}
      
      double num_clusters;
    };
  }
}

#endif
/** @} */ // end of doxygen group 

