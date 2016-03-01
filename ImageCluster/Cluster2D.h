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
  /**
     \class Cluster2D
     User defined class Cluster2D ... these comments are used to generate
     doxygen documentation!
  */
  class Cluster2D{
    
  public:
    
    /// Default constructor
    Cluster2D(){}
    
    /// Default destructor
    ~Cluster2D(){}

    Contour_t _contour;

    std::vector<::cv::Point> _insideHits;
    int _numHits;
    
    Point2D _centerPt;
    Point2D _eigenVecFirst;
    Point2D _eigenVecSecond;

    Point2D _startPt;


    
  };

  typedef std::vector<larcv::Cluster2D> Cluster2DArray_t;
}
  
#endif
/** @} */ // end of doxygen group 

