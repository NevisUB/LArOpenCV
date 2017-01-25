#ifndef __PCACROSSING_H__
#define __PCACROSSING_H__

#include "Geo2D/Core/VectorArray.h"
#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {
 
  class PCACrossing : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    PCACrossing();
    
    /// Default destructor
    virtual ~PCACrossing(){}

    /// Optional configuration method
    void Configure(const Config_t &pset);

  private:
    
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

