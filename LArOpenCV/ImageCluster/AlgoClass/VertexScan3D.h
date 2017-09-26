#ifndef __VERTEXSCAN3D_H__
#define __VERTEXSCAN3D_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArPlaneGeo.h"
#include "Voxel.h"

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
			    const geo2d::Vector<float>& plane_pt,
			    data::CircleVertex& cvtx) const;
    
    bool SetPlanePoint(cv::Mat img,
		       const data::VertexSeed3D& vtx3d,
		       const size_t plane,
		       geo2d::Vector<float>& plane_pt) const;
    
    const larocv::LArPlaneGeo& geo() const { return _geo; }

    size_t num_planes() const { return _geo._num_planes; }

    void RegisterRegions(const std::vector<data::Vertex3D>& vtx3d_v);
    
    data::Vertex3D ScanRegion(const VoxelArray& voxel, const std::vector<cv::Mat>& image_v, size_t num_xspt=0) const;
    
    std::vector<data::Vertex3D> RegionScan3D(const std::vector<cv::Mat>& image_v) const;

    data::CircleVertex RadialScan2D(const cv::Mat& img, const geo2d::Vector<float>& pt) const;

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
    bool _use_circle_weight;
    bool _prohibit_one_xs;
    double _dtheta_cut;
    bool _merge_voxels;
    bool _require_3planes_charge;
    float _allowed_radius;
    std::vector<float> _radius_v;
    
  private:

    double CircleWeight(larocv::data::CircleVertex& cvtx) const;

    void MergeVoxels(std::vector<VoxelArray>& voxel_v);

    larocv::LArPlaneGeo _geo;

    std::vector<VoxelArray> _voxel_vv;
    
  public:
    const std::vector<VoxelArray>& Voxels() const { return  _voxel_vv; }

    const larocv::LArPlaneGeo& Geo() const { return  _geo; }

  };
}

#endif
/** @} */ // end of doxygen group

