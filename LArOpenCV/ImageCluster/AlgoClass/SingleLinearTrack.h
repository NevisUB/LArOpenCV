#ifndef __SINGLELINEARTRACK_H__
#define __SINGLELINEARTRACK_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrack.h"
#include "LArPlaneGeo.h"

/*
  @brief: find compatible 2D edge points across planes to create 3D edge points
*/

namespace larocv {
  
  class SingleLinearTrack : public larocv::laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    SingleLinearTrack() : laropencv_base("SingleLinearTrack")
    {}
    
    /// Default destructor
    virtual ~SingleLinearTrack(){}
    
    void SetPlaneInfo(const ImageMeta& meta) 
    { _geo.ResetPlaneInfo(meta); }

    void Reset()
    { _geo.Reset(); }

    void Configure(const Config_t &pset);
    
    void RegisterVertex2D(const size_t plane,
			  const std::vector<geo2d::Vector<float> >& vtx2d_v);
    
    std::vector<data::LinearTrack3D>
    FindLinearTrack(const std::vector<const cv::Mat>& img_v);

    void
    FindEdges(const cv::Mat& img,
	      const GEO2D_Contour_t& ctor,
	      geo2d::Vector<float>& edge1,
	      geo2d::Vector<float>& edge2) const;
    
    void
    EdgesFromMeanValue(const GEO2D_Contour_t& ctor,
		       geo2d::Vector<float>& edge1,
		       geo2d::Vector<float>& edge2) const;
    
    void
    EdgesFromPCAProjection(const cv::Mat& img,
			   const GEO2D_Contour_t& ctor,
			   geo2d::Vector<float>& edge1,
			   geo2d::Vector<float>& edge2) const;
    
  public:
    std::vector<larocv::data::LinearTrack2D>
    FindLinearTrack2D(const size_t plane, const cv::Mat& img) const;

    void FindLinearTrack(const std::vector<std::vector<larocv::data::LinearTrack2D> >& strack_vv);

    LArPlaneGeo _geo;
    bool _edges_from_mean;
    float _minimum_neighbor_distance;
    size_t _num_planes;
    AlgorithmID_t _algo_id_part;
    //float  _part_pxfrac_threshold;
    size_t _min_size_track_ctor;
    float  _min_length_track_ctor;
    float  _pi_threshold;
    float  _min_compat_dist;
    float _xplane_tick_resolution;
    std::vector<size_t> _seed_plane_v;
    std::vector<std::vector<geo2d::Vector<float> > > _registered_vtx2d_vv;
    
  };

}
#endif
/** @} */ // end of doxygen group 

