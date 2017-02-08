#ifndef __PCACROSSING_H__
#define __PCACROSSING_H__

#include "Geo2D/Core/VectorArray.h"
#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

namespace larocv {
 
  class PCACrossing : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    PCACrossing();
    
    /// Default destructor
    virtual ~PCACrossing(){}

    /// Optional configuration method
    void Configure(const Config_t &pset);

    std::vector<geo2d::Line<float> >
    ComputePCALines(const std::vector<GEO2D_Contour_t>& cluscomp);

    std::vector<geo2d::Line<float> >
    ComputePCALines(const data::TrackClusterCompound& cluscomp);
    
    /// Calculate PCA lines and compute intersections
    std::vector<geo2d::Vector<float> >
    ComputeIntersections(const std::vector<geo2d::Line<float> >& line_v,
			 const cv::Mat& img);
    
    std::vector<geo2d::Vector<float> >
    ComputeIntersections(const std::vector<GEO2D_Contour_t>& cluscomp,
			 const cv::Mat& img);

    std::vector<geo2d::Vector<float> >
    ComputeIntersections(const data::TrackClusterCompound& cluscomp,
    			 const cv::Mat& img);
    
    void
    FilterIntersections(std::vector<geo2d::Vector<float> >& pts_v,
			const cv::Mat& img);
      
    /// Configuration 
    float _dist_to_pixel;
    bool _filter_intersections;
    int _filter_px_val_thresh;
    
  private:
    
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

