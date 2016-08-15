/**
 * \file ROI.h
 *
 * \ingroup LArCaffe
 * 
 * \brief Class def header for a class ROI
 *
 * @author ariana hackenburg 
 */

/** \addtogroup LArCaffe

    @{*/
#ifndef __LARCAFFE_ROI_H__
#define __LARCAFFE_ROI_H__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "larbys.h"
#include "LArOCVTypes.h"

namespace larocv {

//  class Image2D;
  /**
     \class ROI
     A simple class to store image's roi info including\n
     0) origin (left-bottom corner of the picture) absolute coordinate \n
     1) horizontal and vertical size (width and height) in double precision \n
     2) vertex
     It is meant to be associated with a specific cv::Mat or larocv::Image2D object \n
     (where the latter class contains ROI as an attribute).  \n
  */
  class ROI{

    friend class Image2D;

  public:
    
    /// Default constructor: width, height, and origin coordinate won't be modifiable 
    ROI( const double width=0., const double height=0.,
         const double origin_x=0., const double origin_y=0.,
         const size_t plane=::larocv::kINVALID_SIZE)
      : _origin(origin_x,origin_y)
      , _width(width)
      , _height(height)
      , _plane(plane)
      , _roi_vtx(::larocv::kINVALID_DOUBLE,::larocv::kINVALID_DOUBLE)
    {
      if( width  < 0. ) throw larbys("Width must be a positive floating point!");
      if( height < 0. ) throw larbys("Height must be a positive floating point!");
    }
    
    /// Default destructor
    ~ROI(){}

    const Point2D& origin   () const { return _origin; }
    size_t plane            () const { return _plane;  }
    double width            () const { return _width;  }
    double height           () const { return _height; }

    void setorigin(size_t w, size_t t)
    { _origin = larocv::Point2D(w,t); }

    /// Set vertex in ROI, may or may not exists, user should check this
    void setvtx(size_t w, size_t t)
    { _roi_vtx = larocv::Point2D(w,t); }

    void setbounds(std::vector<larocv::Point2D> bounds)
    { _roi_bounds = bounds; }

    /// Get vertex ROI
    larocv::Point2D roivtx() const { return _roi_vtx; }

    std::vector<larocv::Point2D> roibounds() const { return _roi_bounds; }

   protected:

    larocv::Point2D _origin; ///< Absolute coordinate of the left bottom corner of ROI  
    double _width;             ///< Horizontal size of ROI 
    double _height;            ///< Vertical size of ROI 
    size_t _plane;             ///< unique plane ID number
    std::vector<larocv::Point2D> _roi_bounds ; ///< Corners of ROI

    larocv::Point2D _roi_vtx;
    
  };

}

namespace larocv {
  /**
     \class larocv::ROI 
     @brief Borrowed from larocv (see larocv::ROI)
  */
  typedef ::larocv::ROI ROI;

}

#endif
/** @} */ // end of doxygen group 

