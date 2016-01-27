/**
 * \file ImageMeta.h
 *
 * \ingroup LArCaffe
 * 
 * \brief Class def header for a class ImageMeta
 *
 * @author kazuhiro
 */

/** \addtogroup LArCaffe

    @{*/
#ifndef __LARCAFFE_IMAGEMETA_H__
#define __LARCAFFE_IMAGEMETA_H__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "larbys.h"
#include "LArCVTypes.h"

namespace larcaffe {

  class Image2D;
  /**
     \class ImageMeta
     A simple class to store image's meta data including\n
     0) origin (left-bottom corner of the picture) absolute coordinate \n
     1) horizontal and vertical size (width and height) in double precision \n
     2) number of horizontal and vertical pixels \n
     It is meant to be associated with a specific cv::Mat or larcv::Image2D object \n
     (where the latter class contains ImageMeta as an attribute). For cv::Mat, there \n
     is a function ImageMeta::update to constantly update vertical/horizontal # pixels \n
     as it may change in the course of matrix operation.
  */
  class ImageMeta{

    friend class Image2D;

  public:
    
    /// Default constructor: width, height, and origin coordinate won't be modifiable 
    ImageMeta(const double width=0., const double height=0.,
	      const size_t width_npixel=0., const size_t height_npixel=0,
	      const double origin_x=0., const double origin_y=0.)
      : _origin(origin_x,origin_y)
      , _width(width)
      , _height(height)
    {
      if( width  < 0. ) throw larbys("Width must be a positive floating point!");
      if( height < 0. ) throw larbys("Height must be a positive floating point!");
      update(width_npixel,height_npixel);
    }
    
    /// Default destructor
    ~ImageMeta(){}

    const Point2D& origin   () const { return _origin; }
    double width            () const { return _width;  }
    double height           () const { return _height; }
    size_t num_pixel_row    () const { return _width_npixel;      }
    size_t num_pixel_column () const { return _height_npixel;     }
    double pixel_width      () const { return (_width_npixel ? _width / _width_npixel : 0.);     } 
    double pixel_height     () const { return (_height_npixel ? _height / _height_npixel : 0.);   }
    /// Provide relative horizontal coordinate of the center of a specified pixel row
    double pixel_relative_x(size_t w) const { return pixel_width  () * ((double)w + 0.5); }
    /// Provide relative vertical coordinate of the center of a specified pixel column
    double pixel_relative_y(size_t h) const { return pixel_height () * ((double)h + 0.5); }
    /// Provide absolute horizontal coordinate of the center of a specified pixel row
    double pixel_x  (size_t w) const { return _origin.x + pixel_relative_x(w); }
    /// Provide absolute vertical coordinate of the center of a specified pixel row
    double pixel_y  (size_t h) const { return _origin.y + pixel_relative_y(h); }
    /// Change # of vertical/horizontal pixels in meta data
    void update(const size_t width_npixel, const size_t height_npixel)
    {
      //if( !width_npixel  ) throw larbys("Number of pixels must be non-zero (width)");
      //if( !height_npixel ) throw larbys("Number of pixels must be non-zero (height)");
      _width_npixel  = width_npixel;
      _height_npixel = height_npixel;
    }
    /// Change # of vertical/horizontal pixels in meta data with cv::Mat as an input
    void update(const ::cv::Mat& mat)
    { update(mat.rows,mat.cols); }
    
  protected:

    larcaffe::Point2D _origin; ///< Absolute coordinate of the left bottom corner of an image
    double _width;             ///< Horizontal size of an image in double floating precision (in original coordinate unit size)
    double _height;            ///< Vertical size of an image in double floating precision (in original coordinate unit size)
    size_t _width_npixel;      ///< # of pixels in horizontal axis
    size_t _height_npixel;     ///< # of pixels in vertical axis

  };

}

namespace larcv {
  /**
     \class larcv::ImageMeta 
     @brief Borrowed from larcaffe (see larcaffe::ImageMeta)
  */
  typedef ::larcaffe::ImageMeta ImageMeta;

}

#endif
/** @} */ // end of doxygen group 

