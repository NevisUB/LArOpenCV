#ifndef VERTEXSCAN3D_H
#define VERTEXSCAN3D_H

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArPlaneGeo.h"

/*
@brief: scan 3D space for compatible pixels
*/

namespace larocv {

  class Voxel {

  public:

    Voxel(){}
    ~Voxel(){}
    std::vector<float> x_v;
    std::vector<float> y_v;
    std::vector<float> z_v;

    size_t nx() const { return x_v.size(); }
    size_t ny() const { return y_v.size(); }
    size_t nz() const { return z_v.size(); }
    
    data::Vertex3D pt(size_t i, size_t j, size_t k) const {
      if (i >= nx()) throw larbys("i out of x range");
      if (j >= ny()) throw larbys("j out of y range");
      if (k >= nz())  throw larbys("k out of y range");

      data::Vertex3D res;
      res.x = x_v[i];
      res.y = y_v[j];
      res.z = z_v[k];
      return res;
    }
    
    float min_x() const { return *(std::min_element(std::begin(x_v),std::end(x_v))); }
    float min_y() const { return *(std::min_element(std::begin(y_v),std::end(y_v))); }
    float min_z() const { return *(std::min_element(std::begin(z_v),std::end(z_v))); }

    float max_x() const { return *(std::max_element(std::begin(x_v),std::end(x_v))); }
    float max_y() const { return *(std::max_element(std::begin(y_v),std::end(y_v))); }
    float max_z() const { return *(std::max_element(std::begin(z_v),std::end(z_v))); }

    bool Overlap(const Voxel& voxel) const {

      if ((this->max_x() < voxel.min_x()) or (this->min_x() > voxel.max_x())) return false;
      if ((this->max_y() < voxel.min_y()) or (this->min_y() > voxel.max_y())) return false;
      if ((this->max_z() < voxel.min_z()) or (this->min_z() > voxel.max_z())) return false;
      
      return true;
    }

    Voxel Merge(const Voxel& voxel) {
      Voxel res;

      res.x_v.reserve(this->nx() + voxel.nx());
      res.y_v.reserve(this->ny() + voxel.ny());
      res.z_v.reserve(this->nz() + voxel.nz());
      
      for(auto v : this->x_v) res.x_v.push_back(v);
      for(auto v : this->y_v) res.y_v.push_back(v);
      for(auto v : this->z_v) res.z_v.push_back(v);

      for(auto v : voxel.x_v) res.x_v.push_back(v);
      for(auto v : voxel.y_v) res.y_v.push_back(v);
      for(auto v : voxel.z_v) res.z_v.push_back(v);

      return res;
    }
    
  };
  
  
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
    
    data::Vertex3D ScanRegion(const Voxel& voxel, const std::vector<cv::Mat>& image_v, size_t num_xspt=0) const;
    
    std::vector<data::Vertex3D> RegionScan3D(const std::vector<cv::Mat>& image_v) const;

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
    
  private:

    data::CircleVertex RadialScan2D(const cv::Mat& img, const geo2d::Vector<float>& pt) const;

    double CircleWeight(const larocv::data::CircleVertex& cvtx) const;

    void MergeVoxels(std::vector<Voxel>& voxel_v);

    larocv::LArPlaneGeo _geo;

    std::vector<Voxel> _voxel_v;

        

    
  };
}

#endif
/** @} */ // end of doxygen group

