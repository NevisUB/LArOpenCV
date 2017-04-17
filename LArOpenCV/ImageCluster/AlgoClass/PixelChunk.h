#ifndef LAROCV_PIXELCHUNK_H
#define LAROCV_PIXELCHUNK_H

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "Geo2D/Core/Line.h"

namespace larocv {

  enum class ChunkType_t { kUnknown, kTrack, kShower };
  
  struct PixelChunk {
    PixelChunk()  { Reset(); }
    ~PixelChunk() {}
    
    larocv::GEO2D_Contour_t ctor;
    geo2d::Vector<float> edge1;
    geo2d::Vector<float> edge2;
    float length;
    float width;
    float perimeter;
    float area;
    uint npixel;
    geo2d::Line<float> ctorPCA;
    geo2d::Line<float> pixelPCA;
    float track_frac;
    float shower_frac;
    double mean_pixel_dist;
    double sigma_pixel_dist;
    double angular_sum;
    ChunkType_t type;
    
    void BasicFill(const GEO2D_Contour_t& contour);
    void BasicImageFill(const GEO2D_Contour_t& contour, cv::Mat& adc_img, float threshold=0.0);
    void AdvancedImageFill(const GEO2D_Contour_t& contour, cv::Mat& adc_img, cv::Mat& trk_img, cv::Mat& shr_img, float threshold=0.0);
    
    void Reset() {
      ctor       = GEO2D_Contour_t();
      edge1      = geo2d::Vector<float>(kINVALID_FLOAT,kINVALID_FLOAT);
      edge2      = edge1;
      length     = kINVALID_FLOAT;
      width      = kINVALID_FLOAT;
      perimeter  = kINVALID_FLOAT;
      area       = kINVALID_FLOAT;
      npixel     = kINVALID_UINT;
      ctorPCA  = geo2d::Line<float>(edge1,kINVALID_FLOAT);
      pixelPCA = ctorPCA;
      track_frac = kINVALID_FLOAT;
      shower_frac= kINVALID_FLOAT;
      mean_pixel_dist  = kINVALID_FLOAT;
      sigma_pixel_dist = kINVALID_FLOAT;
      angular_sum = kINVALID_DOUBLE;
      type       = ChunkType_t::kUnknown;
    }

    
  };

}
#endif
