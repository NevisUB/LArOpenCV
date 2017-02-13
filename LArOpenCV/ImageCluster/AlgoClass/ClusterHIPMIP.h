#ifndef __CLUSTERHIPMIP_H__
#define __CLUSTERHIPMIP_H__

#include "Geo2D/Core/VectorArray.h"
#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

/*
  @brief: isolate HIP and MIP pixels into seprate contours
*/

namespace larocv {
 
  class ClusterHIPMIP : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ClusterHIPMIP();
    
    /// Default destructor
    virtual ~ClusterHIPMIP(){}

    /// Optional configuration method
    void Configure(const Config_t &pset);

    /// Main function to separate hip and mip contours
    std::pair<std::vector<GEO2D_Contour_t>, std::vector<GEO2D_Contour_t> >
    IsolateHIPMIP(const ::cv::Mat& img);


    const cv::Mat& get_mip_masked_img() { return _mip_thresh_mask_m; }
    const cv::Mat& get_mip_thresh_img() { return _mip_thresh_m;      }
    const cv::Mat& get_hip_thresh_img() { return _hip_thresh_m;      }
    
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
    cv::Mat _mip_thresh_mask_m;
    cv::Mat _mip_thresh_m;
    cv::Mat _hip_thresh_m;
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

