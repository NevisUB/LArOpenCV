#ifndef __VERTEXANALYSIS_H__
#define __VERTEXANALYSIS_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"

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
    RequireParticleCount(std::vector<std::vector<data::ParticleCluster> >& pars_vv,
			 uint nplanes,
			 uint nxs);

    bool
    RequireCrossing(const data::Vertex3D& vtx3d,
		    uint nplanes,
		    uint nxs);

    bool
    MatchExists(std::vector<std::vector<data::ParticleCluster> >& pars_vv,
		const cv::Mat& img,
		float threshold,
		size_t required_per_plane);
  private:
  private:
  private:
  private:
  private:
    
    
  };
  
}
#endif
/** @} */ // end of doxygen group 

