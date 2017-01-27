#ifndef __LINEARPARTICLECLUSTER_H__
#define __LINEARPARTICLECLUSTER_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrackClusterData.h"


namespace larocv {
 
  class LinearParticleCluster : public laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    LinearParticleCluster();
    
    /// Default destructor
    virtual ~LinearParticleCluster(){}

    /// Optional configuration method
    void Configure(const Config_t &pset);



    /// Public configuration
    bool _edges_from_mean;
    float  _pi_threshold;
        
    float _minimum_neighbor_distance;
    size_t _num_planes;
    AlgorithmID_t _algo_id_part;
    float  _part_pxfrac_threshold;
    size_t _min_size_track_ctor;
    float  _min_length_track_ctor;
    float  _min_compat_dist;
    float _xplane_tick_resolution;
    std::vector<size_t> _seed_plane_v;
    float  _trigger_tick;
    std::vector<float> _tick_offset_v;
    std::vector<float> _wire_comp_factor_v;
    std::vector<float> _time_comp_factor_v;
    geo2d::VectorArray<float> _origin_v;


    
  private:

    void FindEdges(const cv::Mat& img,
		   const GEO2D_Contour_t& ctor,
		   geo2d::Vector<float>& edge1,
		   geo2d::Vector<float>& edge2) const;
    
    void EdgesFromMeanValue(const GEO2D_Contour_t& ctor,
			    geo2d::Vector<float>& edge1,
			    geo2d::Vector<float>& edge2) const;
    
    void EdgesFromPCAProjection(const cv::Mat& img,
				const GEO2D_Contour_t& ctor,
				geo2d::Vector<float>& edge1,
				geo2d::Vector<float>& edge2) const;


    std::vector<larocv::data::LinearTrack2D>
    FindLinearTrack2D(const cv::Mat& img) const;

    data::LinearTrackArray
    FindLinearTrack(const std::vector<std::vector<larocv::data::LinearTrack2D> >& strack_vv);
    
    LArPlaneGeo _geo;

    
  };
  
}
#endif
/** @} */ // end of doxygen group 

