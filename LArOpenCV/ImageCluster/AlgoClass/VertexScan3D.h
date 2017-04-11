#ifndef VERTEXSCAN3D_H
#define VERTEXSCAN3D_H

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArPlaneGeo.h"

/*
@brief: scan 3D space for compatible pixels
*/

namespace larocv {

  class VertexScan3D : public laropencv_base {

  public:

    /// Default constructor
    VertexScan3D() : laropencv_base("VertexScan3D")
    {
      _geo._num_planes = 3;
      _geo._trigger_tick = 3200;
      _geo._xplane_tick_resolution = 2;
      _dx = _dy = _dz = 3.0;
      _step_size = 0.3;
    }

    /// Default destructor
    ~VertexScan3D() {}

    void SetPlaneInfo(const larocv::ImageMeta& meta)
    { _geo.ResetPlaneInfo(meta); }

    void Configure(const Config_t &pset);
    
    bool CreateCircleVertex(cv::Mat img,
			    const data::VertexSeed3D& vtx3d,
			    const size_t plane,
			    data::CircleVertex& cvtx) const;

    larocv::data::Vertex3D
    RegionScan3D(const larocv::data::VertexSeed3D& center,
                 std::vector<cv::Mat> image_v,
                 size_t num_xspt=0 ) const;

    const larocv::LArPlaneGeo& geo() const { return _geo; }

    size_t num_planes() const { return _geo._num_planes; }

    float _dx;
    float _dy;
    float _dz;
    float _step_size;
    float _step_radius;
    float _min_radius;
    float _max_radius;
    float _pi_threshold;
    float _angle_supression;
    size_t _pca_box_size;
    
  private:

    data::CircleVertex RadialScan2D(const cv::Mat& img, const geo2d::Vector<float>& pt) const;

    double CircleWeight(const larocv::data::CircleVertex& cvtx) const;

    larocv::LArPlaneGeo _geo;
  };
}

#endif
/** @} */ // end of doxygen group

