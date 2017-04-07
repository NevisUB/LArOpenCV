#ifndef __ONETRACKONESHOWER_H__
#define __ONETRACKONESHOWER_H__

#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"

/*
@brief: chooses 3D vertex compatible with one track, and one track assumption. Uses QPoint crossing @ 2D vertex across
        planes to choose 3D shower candidate seed
*/

namespace larocv {

  class OneTrackOneShower : public larocv::laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    OneTrackOneShower() :
      laropencv_base("OneTrackOneShower"),
      _geo_algo()
    {}
    
    /// Default destructor
    virtual ~OneTrackOneShower(){}

    const std::vector<data::Vertex3D>& ScannedVertex3D() const
    { return _shower_vtx3d_v; }
    
    /// Inherited class configuration method
    void Configure(const Config_t &pset);

    void Reset();
    
    void SetPlaneInfo(const ImageMeta& meta);

    std::vector<data::Vertex3D>
    CreateSingleShower(std::vector<cv::Mat>& img_v);

    void RegisterSeed(const std::vector<data::VertexSeed3D>& seed_v)
    { _cand_vertex_v = seed_v; }

    void
    ValidateCircleVertex(cv::Mat& img,data::CircleVertex& cvtx) const;

    const float circle_default_size() const
    { return _circle_default_radius; }
    
  private:

    geo2d::VectorArray<float>
    ValidShowerPointOnCircle(::cv::Mat& img,
			     const geo2d::Circle<float>& circle,
			     const std::vector<geo2d::Vector<float> >& xs_pts) const;

    std::vector<data::Vertex3D>
    ListShowerVertex(std::vector<cv::Mat>& img_v,
		     const std::vector<data::VertexSeed3D>& cand_v) const;

    LArPlaneGeo _geo_algo;
    
    std::vector<data::VertexSeed3D> _cand_vertex_v;
    
    float  _circle_default_radius;
    size_t _valid_xs_npx;
    size_t _num_planes;
    float  _pi_threshold;
    float  _xplane_tick_resolution;
    
    std::vector<size_t> _seed_plane_v;

    std::vector<data::Vertex3D> _shower_vtx3d_v;
    
    bool _grad_circle;
    float _grad_circle_min;
    float _grad_circle_max;
    float _grad_circle_step;
    std::vector<float> _grad_circle_rad_v;
    bool _require_unique;
    bool _path_exists_check;
    bool _use_seed_radius;
    bool _refine_qpoint;
    float _refine_qpoint_maskout;
    
  };

}
#endif
/** @} */ // end of doxygen group 

