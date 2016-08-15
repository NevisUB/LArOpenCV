#include "Image2D.h"
#include "larbys.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

namespace larocv {

  Image2D::Image2D(size_t width_npixel, size_t height_npixel )
    : std::vector<float>(height_npixel*width_npixel,0.)
    , _meta(0.,0.,width_npixel,height_npixel,0.,0.)
  {}

  Image2D::Image2D(const ImageMeta& meta)
    : std::vector<float>(meta.num_pixel_row()*meta.num_pixel_column(),0.)
    , _meta(meta)
  {}

  Image2D::Image2D(const Image2D& rhs) 
    : std::vector<float>(rhs)
    , _meta(rhs._meta)
  {}

  void Image2D::resize(size_t width_npixel, size_t height_npixel ) 
  {
    std::vector<float>::resize(height_npixel*width_npixel);
    _meta.update(width_npixel,height_npixel);
  }
  
  void Image2D::clear() {
    std::vector<float>::clear();
    _meta.update(1,1);
    //std::cout << "[Image2D (" << this << ")] Cleared image memory " << std::endl;
  }

  void Image2D::clear_data() { for(auto& v : (*this)) v = 0.; }

  void Image2D::set_pixel( size_t w, size_t h, float value ) {
    if ( h >= _meta._height_npixel || w >= _meta._width_npixel ) throw larbys("Out-of-bound pixel set request!");
      return;
    (*this)[w*_meta._height_npixel + h] = value;
  }

  void Image2D::paint(float value)
  { for(auto& v : (*this)) v=value; }
  
  float Image2D::pixel( size_t w, size_t h ) const 
  { return (*this)[index(h,w)]; }

  size_t Image2D::index( size_t w, size_t h ) const {
    
    if ( !isInBounds( h, w ) ) throw larbys("Invalid pixel index queried");
    
    return ( w * _meta._height_npixel + h );
  }

  void Image2D::copy(size_t w, size_t h, const float* src, size_t num_pixel) 
  { 
    const size_t idx = index(h,w);
    if(idx+num_pixel >= size()) throw larbys("memcpy size exceeds allocated memory!");
    
    memcpy(&((*this)[idx]),src, num_pixel * sizeof(float));

  }

  void Image2D::copy(size_t w, size_t h, const std::vector<float>& src, size_t num_pixel) 
  {
    if(!num_pixel)
      this->copy(h,w,(float*)(&(src[0])),src.size());
    else if(num_pixel < src.size()) 
      this->copy(h,w,(float*)&src[0],num_pixel);
    else
      throw larbys("Not enough pixel in source!");
  }

  void Image2D::copy(size_t w, size_t h, const short* src, size_t num_pixel) 
  {
    const size_t idx = index(h,w);
    if(idx+num_pixel >= size()) throw larbys("memcpy size exceeds allocated memory!");
    
    for(size_t i=0; i<num_pixel; ++i) (*this)[idx+i] = src[num_pixel];

  }

  void Image2D::copy(size_t w, size_t h, const std::vector<short>& src, size_t num_pixel) 
  {
    if(!num_pixel)
      this->copy(h,w,(short*)(&(src[0])),src.size());
    else if(num_pixel < src.size()) 
      this->copy(h,w,(short*)&src[0],num_pixel);
    else
      throw larbys("Not enough pixel in source!");
  }

  Image2D Image2D::copy_compress(size_t width, size_t height) const
  {
    if(_meta._height_npixel % height || _meta._width_npixel % width)
      throw larbys("Compression only possible if height/width are modular 0 of compression factor!");

    size_t width_factor  = _meta._width_npixel  / width;
    size_t height_factor = _meta._height_npixel / height;
    ImageMeta meta(_meta);
    meta.update(height,width);
    Image2D result(meta);

    for(size_t w=0; w<width; ++ w) {
      for(size_t h=0; h<height; ++h) {
	float value = 0;
	//std::cout << w*height << " : " << h << std::endl;
	for(size_t orig_w=w*width_factor; orig_w<(w+1)*width_factor; ++orig_w)
	  for(size_t orig_h=h*height_factor; orig_h<(h+1)*height_factor; ++orig_h) {
	    //std::cout << "    " << (*this)[orig_w * _meta._height_npixel + orig_h] << " @ " << orig_w * _meta._height_npixel + orig_h << std::endl;
	    value += (*this)[orig_w * _meta._height_npixel + orig_h];
	  }
	//std::cout<<std::endl;
	result[w*height + h] = value;
      }
    }
    return result;
  }

  void Image2D::compress(size_t width, size_t height)
  { (*this) = copy_compress(height,width); }
  
}
