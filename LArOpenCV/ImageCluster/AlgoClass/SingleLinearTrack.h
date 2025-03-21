#ifndef __SINGLELINEARTRACK_H__
#define __SINGLELINEARTRACK_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrack.h"
#include "LArPlaneGeo.h"
#include "DefectBreaker.h"

/*
  @brief: find compatible 2D edge points across planes to create 3D edge points
*/

namespace larocv {
  
  class SingleLinearTrack : public larocv::laropencv_base {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    SingleLinearTrack() :
      laropencv_base("SingleLinearTrack"),
      _geo()
    {}
    
    /// Default destructor
    virtual ~SingleLinearTrack(){}
    
    void SetPlaneInfo(const ImageMeta& meta) 
    { _geo.ResetPlaneInfo(meta); }

    void Reset()
    { _geo.Reset(); }

    // COnfigure the class
    void Configure(const Config_t &pset);

    // Register a set of points as 2D vertex candidates
    void RegisterVertex2D(const size_t plane,
			  const std::vector<geo2d::Vector<float> >& vtx2d_v);

    // Find LinearTracks with 3D edges over a set of imates
    std::vector<data::LinearTrack3D>
    FindLinearTrack(std::vector<cv::Mat>& img_v);

    // Find edges of contours
    void
    FindEdges(cv::Mat& img,
	      const GEO2D_Contour_t& ctor,
	      geo2d::Vector<float>& edge1,
	      geo2d::Vector<float>& edge2) const;

    // Find edges of contours using the mean value approximation
    void
    EdgesFromMeanValue(const GEO2D_Contour_t& ctor,
		       geo2d::Vector<float>& edge1,
		       geo2d::Vector<float>& edge2) const;

    // Find edges from projected PCA
    void
    EdgesFromPCAProjection(cv::Mat& img,
			   const GEO2D_Contour_t& ctor,
			   geo2d::Vector<float>& edge1,
			   geo2d::Vector<float>& edge2) const;

    // Find a 2D linear track in the plane
    std::vector<larocv::data::LinearTrack2D>
    FindLinearTrack2D(const size_t plane, cv::Mat& img) const;

    // Find a linear track given a plane wise vector of linear tracks
    void
    FindLinearTrack(const std::vector<std::vector<larocv::data::LinearTrack2D> >& strack_vv);

  private:

    // Algorithm to interface between pixel coordinates and real coordinates
    LArPlaneGeo _geo;
    // Algorithm to break contours into atomic contours
    DefectBreaker _DefectBreaker;
    // Toggle to break incoming contours or not
    bool _break_linear_tracks;
    // Toggle to compute the contour edges from mean value
    bool _edges_from_mean;
    // Minimum neighbor distance between edges/contour
    float _minimum_neighbor_distance;
    // Number of planes
    size_t _num_planes;
    // Minimum number of contour points to search for a contour edge
    size_t _min_size_track_ctor;
    // Minimum length contour to search for a contour edge
    float  _min_length_track_ctor;
    // Pixel intensity threshold to find linear tracks
    float  _pi_threshold;
    // Minimum distance for edge and track to be compatible
    float  _min_compat_dist;
    // Distance between ticks to allow for 3D vertex creation
    float _xplane_tick_resolution;
    // Vector of seed planes
    std::vector<size_t> _seed_plane_v;
    // List of registered vertices
    std::vector<std::vector<geo2d::Vector<float> > > _registered_vtx2d_vv;
    
  };

}
#endif
/** @} */ // end of doxygen group 

