#ifndef __PCACROSSING_H__
#define __PCACROSSING_H__

#include "Core/laropencv_base.h"
#include "Core/VectorArray.h"
#include "FhiclLite/PSet.h"
#include "AlgoFunction/Contour2DAnalysis.h"
#include "AlgoData/DefectClusterData.h"

namespace larocv {
 
  class PCACrossing : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    PCACrossing();
    
    /// Default destructor
    virtual ~PCACrossing(){}

    /// Optional configuration method
    void Configure(const ::fcllite::PSet &pset);

    /// Calculate PCA lines and compute intersections
    std::vector<geo2d::Vector<float> >
    ComputeIntersections(data::ClusterCompound cluscomp,
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

