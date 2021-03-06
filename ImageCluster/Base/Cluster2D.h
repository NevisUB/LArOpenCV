/**
 * \file Cluster2D.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class Cluster2D
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __IMAGECLUSTER_CLUSTER2D_H__
#define __IMAGECLUSTER_CLUSTER2D_H__

#include "ImageClusterTypes.h"
#include "ClusterRecoUtil/Base/Polygon2D.h"

namespace larocv {

  class ImageClusterManager;
  /**
     \class Cluster2D
     User defined class Cluster2D ... these comments are used to generate
     doxygen documentation!
  */

  struct params {
    Point2D startpt;
    Point2D endpt;
    Point2D dir;
    double  dist;
    Point2D vtx;    
  };
    
  class Cluster2D{

    friend ImageClusterManager;
    
  public:
    
    /// Default constructor
    Cluster2D() : _cluster_id(kINVALID_CLUSTER_ID)
		, _image_id(kINVALID_CLUSTER_ID)
		, _plane_id(kINVALID_CLUSTER_ID)
    {}
    
    /// Default destructor
    ~Cluster2D(){}

    /// Unique ID getter
    ClusterID_t ClusterID() const { return _cluster_id; }
    /// Plane ID getter
    size_t PlaneID() const { return _plane_id;}
    /// Image ID getter
    ImageID_t ImageID() const { return _image_id;}
    /// Matched Cluster ID info getter
    const std::vector<larocv::ClusterID_t>& MatchID() const { return _match_v; }
    /// Origin 2D point getter
    const Point2D& Origin() const { return _origin; }
    /// Pixel width size (i.e. X unit length in cm)
    double PixelWidth() const { return _pixel_width; }
    /// Pixel height size (i.e. X unit length in cm)
    double PixelHeight() const { return _pixel_height; }
    /// convert from X variable to Time-Tick
    double XtoTimeTick(double x) const { return  ( (x + 0.5) * _pixel_height ) + _origin.y ; }
    /// convert from y variable to Wire
    double YtoWire(double y) const { return ( (y + 0.5 )* _pixel_width ) + _origin.x ; }
    ///convert the direction to a 2D angle, scaling back from image coordinates x/y to return to wire/tick
    double AngleofDir() const { return std::atan2(_eigenVecFirst.x*_pixel_height,_eigenVecFirst.y*_pixel_width); }


    //copy of the essentials...
    void copy_params(const Cluster2D& incluster) {
      this->_startPt      = incluster._startPt;
      this->_eigenVecFirst= incluster._eigenVecFirst;
      this->reco          = incluster.reco;
      this->roi           = incluster.roi;
    }
    
    //
    // Data attributes that can be modified by algorithms
    //
    /// PDG code
    ParticleType_t _particle_type;
    /// The contour
    Contour_t _contour;

    //
    // Filled by various cluster params calculators
    //

    /// Hits inside the contour
    std::vector<::cv::Point> _insideHits;
    /// Number of hits in the contour
    int _numHits() const { return _insideHits.size(); }
    /// Center point of the cluster
    Point2D _centerPt;
    /// If using PCA, principle eigenvector
    Point2D _eigenVecFirst;
    /// If using PCA, second eigenvector
    Point2D _eigenVecSecond;
    /// If using PCA, principle eigenvalue
    double  _eigenValFirst;
    /// If using PCA, second eigenvalue
    double  _eigenValSecond;
    /// Start point
    Point2D _startPt;
    /// End point
    Point2D _endPt;
    /// Total charge
    double _sumCharge;
    /// Length -- could be of line passing through
    double _length;
    /// Width
    double _width;
    /// Total area
    double _area;
    /// Perimeter
    double _perimeter;
    /// Angle of cluster
    double _angle2D;

    /// Cone contour
    std::vector<::cv::Point> _cone_contour ;

    /// For track shower separtion
    double _min_width;
    double _max_width;

    //ROI parameters
    params roi;
    //Reconstructed
    params reco;
    
    /// Axis aligned bounding box
    ::cv::Rect _boundingBox;

    /// Rotated min area bounding box
    ::cv::RotatedRect _minAreaBox;

    /// Verticies of the line minimum bounding rectangle
    std::vector<::cv::Point2f> _minAreaRect;

    /// Verticies of the BoundingRect
    std::vector<::cv::Point2f> _minBoundingRect;
    
    /// Forgot
    Polygon2D PolyObject;
    
    /// Chosen points as the edge that contains the start point
    std::vector<::cv::Point2f> _chosen;

    /// 2D vertex point
    Point2D _vertex_2D;

    /// Cluster distance to vertex
    float _dist_to_vtx ;

    // Number of pooling-corrected hits
    int _num_hits_pool ;

  private:

    ClusterID_t _cluster_id;
    ImageID_t  _image_id;
    size_t _plane_id;
    std::vector<larocv::ClusterID_t> _match_v;
    Point2D _origin;
    double _pixel_width;
    double _pixel_height;

    
  };

  typedef std::vector<larocv::Cluster2D> Cluster2DArray_t;
  typedef std::vector<const larocv::Cluster2D*> Cluster2DPtrArray_t;
}
  
#endif
/** @} */ // end of doxygen group 

