#ifndef __PIXELSCAN3D_H__
#define __PIXELSCAN3D_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArPlaneGeo.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "Voxel.h"
#include <array>

/*
  @brief: scan 3D space for compatible pixels
*/

namespace larocv {

  class PixelScan3D : public laropencv_base {

  public:

    /// Default constructor
  PixelScan3D() : laropencv_base("PixelScan3D")
      {
	_geo._num_planes = 3;
	_geo._trigger_tick = 3200;
	_geo._xplane_tick_resolution = 2;
      }

    /// Default destructor
    ~PixelScan3D() {}

    void SetPlaneInfo(const larocv::ImageMeta& meta)
    { _geo.ResetPlaneInfo(meta); }

    void Configure(const Config_t &pset);
    void Configure();
    
    bool SetPlanePoint(cv::Mat img,
		       const data::Vertex3D& vtx3d,
		       const size_t plane,
		       geo2d::Vector<float>& plane_pt) const;

    
    std::vector<std::vector<data::Vertex3D> > 
    RegionScan3D(const std::array<cv::Mat,3>& image_v, 
		   const data::Vertex3D& vtx3d) const;
    
    std::vector<std::vector<data::Vertex3D> >
    RegisterRegions(const data::Vertex3D& vtx) const;
    
    std::vector<std::vector<std::array<size_t,3> > > 
    AssociateContours(const std::vector<std::vector<data::Vertex3D> >& reg_vv,
		      const std::array<GEO2D_ContourArray_t,3>& ctor_vv);


    const LArPlaneGeo& geo() const { return _geo; }

  private:
    LArPlaneGeo _geo;
    
    std::vector<float> _radius_v;
    size_t _theta_base;
    size_t _phi_base;
    std::vector<float> _theta_v;
    std::vector<float> _phi_v;

    inline float X(const float r, const float theta, const float phi) const { return r * sin(phi) * sin(theta); }
    inline float Y(const float r, const float theta, const float phi) const { return r * cos(phi) * sin(theta); }
    inline float Z(const float r, const float theta) const { return r * cos(theta); }

  };
}

#endif
/** @} */ // end of doxygen group

