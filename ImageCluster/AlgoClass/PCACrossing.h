#ifndef __PCACROSSING_H__
#define __PCACROSSING_H__

#include "Core/laropencv_base.h"
#include "Core/VectorArray.h"
#include "FhiclLite/PSet.h"
#include "AlgoFunction/Contour2DAnalysis.h"

namespace larocv {
 
  class PCACrossing : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    PCACrossing();
    
    /// Default destructor
    virtual ~PCACrossing(){}

    /// Optional configuration method
    void Configure(const ::fcllite::PSet &pset);

  private:
    
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

