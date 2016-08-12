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

   
  struct ROI{
 
     int index;
     int type;
     int shape;
     int mcst_index;
     int mct_index;
     float energy_dep;
     float energy_init;
     int pdg;
     int parent_pdg;
     int trackid;
     int parent_trackid;
     float x ;
     float y ;
     float z ;
     float t ;
     float px ;
     float py ;
     float pz ;
     float parent_x ;
     float parent_y ;
     float parent_z ;
     float parent_t ;
     float parent_px ;
     float parent_py ;
     float parent_pz ;
     //std::vector<float> vtx3D;
     //std::vector<float> dir3D;
     //std::vector<float> parent_vtx3D;
     //std::vector<float> parent_dir3D;
     int current_type;
     int interaction_type;

     // 
     // This ugly ass initialization could probably handle some house keeping at some point
     // Initializing in the order that parameters appear above
     ROI(int i=-1, int t=-1, int sh=-1, int ms_i=-1, int mt_i=-1, float e_dep = 0,
         float e_init=0, int PDG=-1, int parent_PDG=-1, int tid=-1, int p_tid=-1,
	 float X=0, float Y=0, float Z=0, float T=0, float PX=0, float PY=0, float PZ=0,
	 float par_x=0, float par_y=0, float par_z=0, float par_t=0, float par_px=0,
	 float par_py=0, float par_pz=0, int c_type=-1, int int_type=-1):
	 index(i), type(t), shape(sh), mcst_index(ms_i), mct_index(mt_i), energy_dep(e_dep),
	 energy_init(e_init), pdg(PDG), parent_pdg(parent_PDG), trackid(tid), parent_trackid(p_tid),
	 x(X), y(Y), z(Z), t(T), px(PX), py(PY), pz(PZ), parent_x(par_x), parent_y(par_y), parent_z(par_z),
	 parent_t(par_t), parent_px(par_px), parent_py(par_py), parent_pz(par_pz), current_type(c_type),
	 interaction_type(int_type){}

      };  


  class Image2D;
  /**
     \class ImageMeta
     A simple class to store image's meta data including\n
     0) origin (left-bottom corner of the picture) absolute coordinate \n
     1) horizontal and vertical size (width and height) in double precision \n
     2) number of horizontal and vertical pixels \n
     It is meant to be associated with a specific cv::Mat or larocv::Image2D object \n
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
      update(width_npixel,height_npixel);
    }
    
    /// Default destructor
    ~ImageMeta(){}

    const Point2D& origin   () const { return _origin; }
    size_t plane            () const { return _plane;  }
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

    /// Set vertex from ROI, may or may not exists, user should check this
    void setvtx(size_t w, size_t t)
    { _roi_vtx = larcaffe::Point2D(w,t); }

    void settrkend(size_t w, size_t t)
    { _roi_trkend = larcaffe::Point2D(w,t); }

    /// Get vertex ROI
    larcaffe::Point2D roivtx() const { return _roi_vtx; }
    larcaffe::Point2D roitrkend() const { return _roi_trkend; }

    /// Fill index of roi vector
    void SetLArCVROIs( const std::vector<ROI> r ){ _larcv_roi_v = r; }

    void ClearROIs(){ _larcv_roi_v.clear(); }

    const std::vector<ROI> GetROIs() const { return _larcv_roi_v; }

   protected:

    larcaffe::Point2D _origin; ///< Absolute coordinate of the left bottom corner of an image
    double _width;             ///< Horizontal size of an image in double floating precision (in original coordinate unit size)
    double _height;            ///< Vertical size of an image in double floating precision (in original coordinate unit size)
    size_t _width_npixel;      ///< # of pixels in horizontal axis
    size_t _height_npixel;     ///< # of pixels in vertical axis
    size_t _plane;             ///< unique plane ID number

    /// ROI vertex, may or may not exist, user should check this again
    larcaffe::Point2D _roi_vtx;
    larcaffe::Point2D _roi_trkend;

    std::vector<ROI> _larcv_roi_v ; 
    
  };

}

namespace larocv {
  /**
     \class larocv::ImageMeta 
     @brief Borrowed from larcaffe (see larcaffe::ImageMeta)
  */
  typedef ::larcaffe::ImageMeta ImageMeta;

}

#endif
/** @} */ // end of doxygen group 

