#ifndef __VERTEXPARTICLECLUSTER_H__
#define __VERTEXPARTICLECLUSTER_H__

#include "PiRange.h"
#include "Geo2D/Core/VectorArray.h"
#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex3D.h"

namespace larocv {
 
  class VertexParticleCluster : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexParticleCluster();
    
    /// Default destructor
    virtual ~VertexParticleCluster(){}

    /// Optional configuration method
    void Configure(const Config_t &pset);

    void PrintConfig() const;

    /// Main function to create particle cluster using polar coordinate
    std::vector<GEO2D_Contour_t>
    CreateParticleCluster(const ::cv::Mat& img,
			  const data::CircleVertex& vtx2d,
			  const GEO2D_Contour_t& super_cluster);

    /// Access to seed clusters
    const GEO2D_ContourArray_t&
    SeedClusters() const { return _seed_cluster_v; }

    /// Access to child clusters
    const GEO2D_ContourArray_t&
    ChildClusters() const { return _child_cluster_v; }

    //
    // Public algorithm configuration attributes
    //
    int _dilation_size;
    int _dilation_iter;
    int _blur_size;
    
    float _theta_hi;
    float _theta_lo;
    float _mask_fraction_radius;
    float _mask_min_radius;
    unsigned short _pi_threshold;

    bool   _refine_polar_cluster;
    bool   _refine_cartesian_cluster;
    bool   _merge_by_mask;

    bool   _use_theta_half_angle;
    double _contour_dist_threshold;

    
  private:

    GEO2D_ContourArray_t
    ParticleHypothesis(const ::cv::Mat& img,
		       const data::CircleVertex& vtx);
    
    GEO2D_ContourArray_t _seed_cluster_v;
    GEO2D_ContourArray_t _child_cluster_v;
    PiRange _prange;
  };
  
}
#endif
/** @} */ // end of doxygen group 

