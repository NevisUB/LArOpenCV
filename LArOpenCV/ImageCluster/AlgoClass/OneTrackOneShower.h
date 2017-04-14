#ifndef __ONETRACKONESHOWER_H__
#define __ONETRACKONESHOWER_H__

#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"

/*
@brief: chooses 3D vertex compatible with one track, and one shower assumption. Uses QPoint crossing @ 2D vertex across
        planes to choose 3D shower vertex candidate
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

    /// Reset certain class members
    void Reset();

    /// Inform class members of incoming image dimensions
    void SetPlaneInfo(const ImageMeta& meta);

    /// Given an image array, find likely single showers
    std::vector<data::Vertex3D>
    CreateSingleShower(std::vector<cv::Mat>& img_v);

    /// Register a 3D seed to be searched in 2D for a shower crossing point
    void RegisterSeed(const std::vector<data::VertexSeed3D>& seed_v)
    { _cand_vertex_v = seed_v; }

    /// Given an image, fill out circle vertex info
    void
    ValidateCircleVertex(cv::Mat& img,data::CircleVertex& cvtx) const;

    /// Getter for default radius size
    const float circle_default_size() const
    { return _circle_default_radius; }
    

    /// Given an image array, determine which vertex seeds are likely vertices
    std::vector<data::Vertex3D>
    ListShowerVertex(std::vector<cv::Mat>& img_v,
		     const std::vector<data::VertexSeed3D>& cand_v) const;

    ///  Validate crossing points for a given circle on an image
    geo2d::VectorArray<float>
    ValidShowerPointOnCircle(::cv::Mat& img,
			     const geo2d::Circle<float>& circle,
			     const std::vector<geo2d::Vector<float> >& xs_pts) const;
    
  private:

    // Helper class to convert between image & physical coordinates
    LArPlaneGeo _geo_algo;

    // Internal list of vertex seeds -- many seeds can be studied with
    // multiple calls to Register function
    std::vector<data::VertexSeed3D> _cand_vertex_v;
    
    // Default radius size for circle @ 2D projected vertex (to determine crossing point)
    float  _circle_default_radius;
    // Required number of pixels above threshold between 2D projected vertex
    // and crossing point on circle
    size_t _valid_xs_npx;
    // Number of planes (number of images)
    size_t _num_planes;
    // Pixel intensity threshold for image
    float  _pi_threshold;
    // Cross plane tick resolution to allow possible 3D vertex creation from 2, 2D points
    float  _xplane_tick_resolution;

    // Array of shower vertices after scanning vertex seed
    std::vector<data::Vertex3D> _shower_vtx3d_v;

    // Graduate a circle or not?
    bool _grad_circle;
    // Starting radius
    float _grad_circle_min;
    // Ending radius
    float _grad_circle_max;
    // Radius step
    float _grad_circle_step;
    // Vector of radii to calculate qpoint
    std::vector<float> _grad_circle_rad_v;
    // Require unique crossing point across 2 planes or more to claim vertex
    bool _require_unique;
    // Check for a gap between 2D vertex and circle crossing point
    bool _path_exists_check;
    // Use the VertexSeed2D radius instead of the default
    bool _use_seed_radius;
    // Refine the found crossing points based on the inner charge blob
    bool _refine_qpoint;
    // Radius size to maskout the inner chare blob
    float _refine_qpoint_maskout;
    // Minimum contour size for gap check
    uint _min_ctor_size;
    // Pixel intensity threshold for the image
    float _pi_thr;
    // Pixel tolerance (thickness of circle) to maskout
    float _pi_tol;
    // Distance threshold between point and contour to claim contour is "inside"
    float _d_thresh;
    
  };

}
#endif
/** @} */ // end of doxygen group 

