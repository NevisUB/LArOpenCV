#ifndef __VERTEXANALYSIS_H__
#define __VERTEXANALYSIS_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"

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
    ResetPlaneInfo(const larocv::ImageMeta& meta);
    
    void
    MergeNearby(const std::vector<const data::Vertex3D*>& vtx1_v,
		std::vector<const data::Vertex3D*>& vtx2_v,
		double dist3d);

    void
    MergeNearby(std::vector<const data::Vertex3D*>& vtx1_v,
		double dist3d);
    void
    FilterByCrossing(std::vector<const data::Vertex3D*>& vtx_v,
		     uint nplanes,
		     uint nxs);

    bool
    RequireParticleCount(const std::vector<std::vector<data::ParticleCluster> >& pars_vv,
			 uint nplanes,
			 uint nxs);

    bool
    RequireParticleCount(const std::vector<std::vector<const data::ParticleCluster*> >& pars_vv,
			 uint nplanes,
			 uint nxs);

    bool
    RequireCrossing(const data::Vertex3D& vtx3d,
		    uint nplanes,
		    uint nxs);

    std::vector<std::vector<std::pair<size_t,size_t> > >
    MatchClusters(const std::vector<std::vector<data::ParticleCluster> >& pars_vv,
		  const std::vector<cv::Mat>& img_v,
		  float threshold,
		  size_t required_per_plane,
		  size_t required_matches);

    
    std::vector<std::vector<std::pair<size_t,size_t> > >
    MatchClusters(const std::vector<std::vector<const data::ParticleCluster* > >& pars_ptr_vv,
		  const std::vector<cv::Mat>& img_v,
		  float threshold,
		  size_t required_per_plane,
		  size_t required_matches);
    
    bool
    MatchExists(const std::vector<std::vector<const data::ParticleCluster*> >& pars_ptr_vv,
		const std::vector<cv::Mat>& img_v,
		float threshold,
		size_t required_per_plane,
		size_t required_matches,
		std::vector<std::vector<std::pair<size_t,size_t> > >& match_vv);

    const LArPlaneGeo& Geo() { return _geo; }
    
  private:

    LArPlaneGeo _geo;
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

