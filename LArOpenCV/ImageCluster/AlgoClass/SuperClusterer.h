#ifndef __SUPERCLUSTERER_H__
#define __SUPERCLUSTERER_H__

#include "PiRange.h"
#include "Geo2D/Core/VectorArray.h"
#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexClusterData.h"

namespace larocv {
 
  class SuperClusterer : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    SuperClusterer();
    
    /// Default destructor
    virtual ~SuperClusterer(){}

    /// Optional configuration method
    void Configure(const Config_t &pset);

    void PrintConfig() const;

    /// Main function to create super-cluster
    void CreateSuperCluster(const ::cv::Mat& img, GEO2D_ContourArray_t& super_cluster_v) const;

    //
    // Public algorithm configuration attributes
    //
    int _dilation_size;
    int _dilation_iter;
    int _blur_size;
    unsigned short _pi_threshold;

  };
  
}
#endif
/** @} */ // end of doxygen group 

