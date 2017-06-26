#ifndef __VERTEXANALYSIS_H__
#define __VERTEXANALYSIS_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
#include <array>

/*
  @brief: XXX
*/

namespace larocv {
 
  class VertexAnalysis : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexAnalysis();
    
    /// Default destructor
    virtual ~VertexAnalysis(){}

    /// Optional configuration method
    void Configure(const Config_t &pset);

    void
    ResetGeo();
    
    void
    ResetPlaneInfo(const larocv::ImageMeta& meta);
    
    void
    MergeNearby(const std::vector<const data::Vertex3D*>& vtx1_v,
		std::vector<const data::Vertex3D*>& vtx2_v,
		double dist3d) const;

    void
    MergeNearby(std::vector<data::Vertex3D>& vtx_v,
		double dist3d) const;
    
    void
    FilterByCrossing(std::vector<const data::Vertex3D*>& vtx_v,
		     uint nplanes,
		     uint nxs) const;

    bool
    RequireParticleCount(const std::vector<std::vector<data::ParticleCluster> >& pars_vv,
			 uint nplanes,
			 uint nxs) const;

    bool
    RequireParticleCount(const std::vector<std::vector<const data::ParticleCluster*> >& pars_vv,
			 uint nplanes,
			 uint nxs) const;

    bool
    RequireCrossing(const data::Vertex3D& vtx3d,
		    uint nplanes,
		    uint nxs) const;

    std::vector<std::vector<std::pair<size_t,size_t> > >
    MatchClusters(const std::vector<std::vector<data::ParticleCluster> >& pars_vv,
		  const std::vector<cv::Mat>& img_v,
		  float threshold,
		  size_t required_per_plane,
		  size_t required_matches,
		  bool check_type=true,
		  bool weight_by_size=false) const;
    
    
    std::vector<std::vector<std::pair<size_t,size_t> > >
    MatchClusters(const std::vector<std::vector<const data::ParticleCluster* > >& pars_ptr_vv,
		  const std::vector<cv::Mat>& img_v,
		  float threshold,
		  size_t required_per_plane,
		  size_t required_matches,
		  bool check_type=true,
		  bool weight_by_size=false) const;
    
    bool
    MatchExists(const std::vector<std::vector<const data::ParticleCluster*> >& pars_ptr_vv,
		const std::vector<cv::Mat>& img_v,
		float threshold,
		size_t required_per_plane,
		size_t required_matches,
		std::vector<std::vector<std::pair<size_t,size_t> > >& match_vv,
		bool check_type=true,
		bool weight_by_size=false) const;

    bool
    CheckFiducial(const data::Vertex3D& vtx3d,
		  float edge_x=10.0, float edge_y=10.0, float edge_z=10.0) const;

    bool
    MatchEdge(const std::array<const data::TrackClusterCompound*,3>& tcluster_arr,
	      data::Vertex3D& vertex) const;

    bool
    MatchEdge(const data::TrackClusterCompound& track0, size_t plane0,
	      const data::TrackClusterCompound& track1, size_t plane1,
	      data::Vertex3D& vertex) const;
    
    bool
    MatchEdge(const data::TrackClusterCompound& track0, size_t plane0,
	      const data::TrackClusterCompound& track1, size_t plane1,
	      const data::TrackClusterCompound& track2, size_t plane2,
	      data::Vertex3D& vertex) const;
    
    const LArPlaneGeo& Geo() const { return _geo; }

    data::Vertex3D
    AverageVertex(const data::Vertex3D& vtx1, const data::Vertex3D& vtx2) const;
    
    bool
    UpdatePlanePosition(data::Vertex3D& vtx,const LArPlaneGeo& geo) const;

    bool
    UpdatePlanePosition(data::Vertex3D& vtx) const;

    bool
    UpdatePlanePosition(data::Vertex3D& vtx, const LArPlaneGeo& geo, size_t plane) const;

    bool
    UpdatePlanePosition(data::Vertex3D& vtx, const LArPlaneGeo& geo, std::array<bool,3>& in_image_v) const;
    
  private:

    LArPlaneGeo _geo;
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

