#ifndef __LARPLANEGEO_H__
#define __LARPLANEGEO_H__

#include "Core/laropencv_base.h"
#include "Core/VectorArray.h"
#include "Core/ImageMeta.h"
#include "AlgoData/AlgoDataVertex.h"

namespace larocv {
 
  class LArPlaneGeo : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    LArPlaneGeo();
    
    /// Default destructor
    virtual ~LArPlaneGeo(){}

    /// Explicit re-set function to put all parameters back to constructor default
    void Reset();

    /// Plane parameter configuration method
    void ResetPlaneInfo(const larocv::ImageMeta& meta);

    float y2wire(float y, const size_t plane) const;
    float x2tick(float x, const size_t plane) const;
    float wire2y(float wire, const size_t plane) const;
    float tick2x(float tick, const size_t plane) const;
    bool  YZPoint(const geo2d::Vector<float>& pt0, const size_t plane0,
		  const geo2d::Vector<float>& pt1, const size_t plane1,
		  larocv::data::Vertex3D& result) const;

    float  _xplane_tick_resolution;
    size_t _num_planes;
    float  _trigger_tick;
    
  private:
    std::vector<float> _tick_offset_v;
    std::vector<float> _wire_comp_factor_v;
    std::vector<float> _time_comp_factor_v;
    geo2d::VectorArray<float> _origin_v;
  };
  
}
#endif
/** @} */ // end of doxygen group 
