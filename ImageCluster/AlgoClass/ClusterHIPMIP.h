#ifndef __CLUSTERHIPMIP_H__
#define __CLUSTERHIPMIP_H__

#include "Core/laropencv_base.h"
#include "Core/VectorArray.h"
#include "FhiclLite/PSet.h"
#include "AlgoFunction/Contour2DAnalysis.h"

namespace larocv {
 
  class ClusterHIPMIP : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ClusterHIPMIP();
    
    /// Default destructor
    virtual ~ClusterHIPMIP(){}

    /// Optional configuration method
    void Configure(const ::fcllite::PSet &pset);

    /// Main function to create particle cluster using polar coordinate
    std::pair<std::vector<GEO2D_Contour_t>, std::vector<GEO2D_Contour_t> >
    IsolateHIPMIP(const ::cv::Mat& img);

    //double _contour_dist_threshold;

    int _min_hip_cluster_size;
    int _min_mip_cluster_size;

    int _mip_thresh;
    int _hip_thresh;

    int _dilation_size;
    int _dilation_iter;
    int _blur_size;

    bool _mask_hip;

    int _hip_mask_tolerance;

    
  private:
    
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

