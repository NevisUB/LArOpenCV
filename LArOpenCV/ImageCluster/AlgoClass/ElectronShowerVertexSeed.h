#ifndef __ELECTRONSHOWERVERTEXSEED_H__
#define __ELECTRONSHOWERVERTEXSEED_H__

#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrack.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"

namespace larocv {

  class ElectronShowerVertexSeed : public larocv::laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ElectronShowerVertexSeed() : laropencv_base("ElectronShowerVertexSeed")
    {}
    
    /// Default destructor
    virtual ~ElectronShowerVertexSeed(){}

    /// Inherited class configuration method
    void Configure(const Config_t &pset);

    void Reset();
    
    void SetPlaneInfo(const ImageMeta& meta);

    std::vector<data::Vertex3D> CreateSeed();

    // void RegisterSeed(const std::vector<const cv::Mat>& img_v,
    // 		      const larocv::data::VertexClusterArray& part,
    // 		      const larocv::data::dQdXProfilerData& dqdx);

    void RegisterSeed(const std::vector<const cv::Mat>& img_v,
		      const std::vector<data::VertexTrackInfoCollection>& vtxinfo_v);
    
    void RegisterSeed(const larocv::data::LinearTrack3DArray& data);

  private:

    std::vector<larocv::data::Vertex3D>
    ListCandidateVertex(const std::vector<larocv::data::Vertex3D>& ltrack_vertex_v,
			const std::vector<larocv::data::Vertex3D>& vtrack_vertex_v,
			const std::vector<larocv::data::Vertex3D>& vedge_vertex_v) const;
    
    LArPlaneGeo _geo_algo;

    float _part_pxfrac_threshold;

    float _vertex_min_separation;
    float _circle_default_radius;
    size_t _num_planes;
    float  _pi_threshold;
    float _xplane_tick_resolution;
    
    std::vector<larocv::data::Vertex3D> _ltrack_vertex_v;
    std::vector<larocv::data::Vertex3D> _vtrack_vertex_v;
    std::vector<larocv::data::Vertex3D> _vedge_vertex_v;

  };

}
#endif
/** @} */ // end of doxygen group 

