#ifndef __LARPLANEGEO_H__
#define __LARPLANEGEO_H__

#include "Geo2D/Core/VectorArray.h"
#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/Core/ImageMeta.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"

/*
  @brief: Various functions for image pixel -> physical wire and tick conversion
*/

namespace larocv {
 
  class LArPlaneGeo : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    LArPlaneGeo();
    
    /// Default destructor
    virtual ~LArPlaneGeo(){}

    /// Configuration
    void Configure(const Config_t &pset);

    /// Explicit re-set function to put all parameters back to constructor default
    void Reset();

    /// Plane parameter configuration method
    void ResetPlaneInfo(const larocv::ImageMeta& meta);

    float tick_offset(const size_t plane) const;
    float x_offset(const size_t plane) const;

    float x2col(float x, size_t plane) const;
    float yz2row(float y, float z, size_t plane) const;
    float row2wire(float y, const size_t plane) const;
    float col2tick(float x, const size_t plane) const;
    float col2cm  (float x, const size_t plane) const;
    float wire2row(float wire, const size_t plane) const;
    float tick2col(float tick, const size_t plane) const;

    bool YZPoint(const geo2d::Vector<float>& pt0, const size_t plane0,
		 const geo2d::Vector<float>& pt1, const size_t plane1,
		 larocv::data::Vertex3D& result) const;
    
    bool YZPoint(const geo2d::Vector<float>& pt0, const size_t plane0,
		 const geo2d::Vector<float>& pt1, const size_t plane1) const;

    float  _xplane_tick_resolution;
    size_t _num_planes;
    float  _trigger_tick;
    
  private:
    std::vector<float> _tick_offset_v;
    std::vector<float> _wire_comp_factor_v;
    std::vector<float> _time_comp_factor_v;
    std::vector<float> _num_cols_v;
    std::vector<float> _num_rows_v;
    geo2d::VectorArray<float> _origin_v;
  };
  
}
#endif
/** @} */ // end of doxygen group 

