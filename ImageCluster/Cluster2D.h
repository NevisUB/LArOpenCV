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

namespace larcv {

  class ImageClusterManager;
  /**
     \class Cluster2D
     User defined class Cluster2D ... these comments are used to generate
     doxygen documentation!
  */
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
    const std::vector<larcv::ClusterID_t>& MatchID() const { return _match_v; }
    
    Contour_t _contour;

    std::vector<::cv::Point> _insideHits;
    int _numHits;
    
    Point2D _centerPt;
    Point2D _eigenVecFirst;
    Point2D _eigenVecSecond;
    Point2D _startPt;
    
  private:
    ClusterID_t _cluster_id;
    ImageID_t   _image_id;
    size_t _plane_id;
    std::vector<larcv::ClusterID_t> _match_v;
  };

  typedef std::vector<larcv::Cluster2D> Cluster2DArray_t;
  typedef std::vector<const larcv::Cluster2D*> Cluster2DPtrArray_t;
}
  
#endif
/** @} */ // end of doxygen group 

