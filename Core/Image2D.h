/**
 * \file Image2D.h
 *
 * \ingroup LArCaffe
 * 
 * \brief Class def header for an image data holder
 *
 * @author tmw, kazu
 */

/** \addtogroup LArCaffe

    @{*/

#ifndef __LAROCV_Image2D_H__
#define __LAROCV_Image2D_H__

#include <vector>
#include <cstdlib>
#include "ImageMeta.h"

namespace larocv {
#include "ROI.h"

  /**
     \class Image2D
     Meant to be a storage class for an image into a ROOT file. Not yet implemented (don't bother reading!)
  */
  class Image2D : protected std::vector<float> {
    
  public:
    
    Image2D(size_t width_npixel=0, size_t height_npixel=0);
    Image2D(const ImageMeta&);
    Image2D(const Image2D&);
    virtual ~Image2D(){}

    unsigned int height() const { return _meta._height_npixel; }
    unsigned int width()  const { return _meta._width_npixel;  }
    float pixel(size_t w, size_t h) const;
    inline bool  isInBounds( size_t w, size_t h ) const
    { return ( h < _meta._height_npixel && w < _meta._width_npixel ); }
    Image2D copy_compress(size_t width_npixel, size_t height_npixel) const;
    size_t index( size_t w, size_t h ) const;

    void copy(size_t w, size_t h, const float* src, size_t num_pixel);
    void copy(size_t w, size_t h, const std::vector<float>& src, size_t num_pixel=0);
    void copy(size_t w, size_t h, const short* src, size_t num_pixel);
    void copy(size_t w, size_t h, const std::vector<short>& src, size_t num_pixel=0);
    const std::vector<float>& as_vector() const 
    { return (*this); }

    const ImageMeta& meta_data() const { return _meta; }
    const ROI& roi_data() const { return _roi; }
    
    void resize( size_t width_npixel, size_t height_npixel );
    void set_pixel( size_t w, size_t h, float value );
    void paint(float value);
    void clear_data();
    void compress(size_t width_npixel, size_t height_npixel);

  private:
    ImageMeta _meta;
    ROI       _roi ;

    void clear();
  };
}

#endif
/** @} */ // end of doxygen group 
