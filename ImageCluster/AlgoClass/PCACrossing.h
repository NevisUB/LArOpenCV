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

    std::vector<geo2d::Line<float> >
    ComputePCALines(const std::vector<GEO2D_Contour_t>& cluscomp);

    /// Calculate PCA lines and compute intersections
    std::vector<geo2d::Vector<float> >
    ComputeIntersections(const std::vector<geo2d::Line<float> >& line_v,
			 const cv::Mat& img);
      
    std::vector<geo2d::Vector<float> >
    ComputeIntersections(const std::vector<GEO2D_Contour_t>& cluscomp,
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

