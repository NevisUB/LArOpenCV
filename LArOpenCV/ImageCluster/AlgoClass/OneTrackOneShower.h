#ifndef __ONETRACKONESHOWER_H__
#define __ONETRACKONESHOWER_H__

#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/SingleShowerData.h"
#include "LArOpenCV/ImageCluster/AlgoData/dQdXProfilerData.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexClusterData.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrackClusterData.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"

namespace larocv {
 
  class OneTrackOneShower : public larocv::laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    OneTrackOneShower() : laropencv_base("OneTrackOneShower")
    {}
    
    /// Default destructor
    virtual ~OneTrackOneShower(){}

    const std::vector<larocv::data::Vertex3D>& ScannedVertex3D() const
    { return _shower_vtx3d_v; }
    
    const std::vector<std::vector<larocv::data::CircleVertex> >& ScannedVertex2D() const
    { return _shower_vtx2d_vv; }

    /// Inherited class configuration method
    void Configure(const Config_t &pset);

    void Reset();
    
    void SetPlaneInfo(const ImageMeta& meta);

    std::vector<data::SingleShower>
    CreateSingleShower(const std::vector<const cv::Mat>& img_v);

    void RegisterSeed(const std::vector<data::Vertex3D>& seed_v)
    { _cand_vertex_v = seed_v; }

  private:

    larocv::data::ShowerCluster
    SingleShowerHypothesis(const ::cv::Mat& img,
			   const size_t plane,
			   const data::CircleVertex& vtx);
    geo2d::VectorArray<float>
    ValidShowerPointOnCircle(const ::cv::Mat& img,
			     const geo2d::Circle<float>& circle,
			     const std::vector<geo2d::Vector<float> >& xs_pts) const;
    
    void ListShowerVertex(const std::vector<const cv::Mat>& img_v,
			  const std::vector<larocv::data::Vertex3D>& cand_v,
			  std::vector<larocv::data::Vertex3D>& res_vtx_v,
			  std::vector<std::vector<larocv::data::CircleVertex> >& res_cvtx_v) const;

    LArPlaneGeo _geo_algo;
    std::vector<larocv::data::Vertex3D> _cand_vertex_v;

    float _theta_lo;
    float _theta_hi;

    float _circle_default_radius;
    size_t _valid_xs_npx;
    size_t _num_planes;
    float  _pi_threshold;
    float _xplane_tick_resolution;
    std::vector<size_t> _seed_plane_v;

    std::vector<larocv::data::Vertex3D> _shower_vtx3d_v;
    std::vector<std::vector<larocv::data::CircleVertex> > _shower_vtx2d_vv;

  };

}
#endif
/** @} */ // end of doxygen group 

