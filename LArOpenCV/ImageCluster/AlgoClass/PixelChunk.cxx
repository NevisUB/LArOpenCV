#ifndef LAROCV_PIXELCHUNK_CXX
#define LAROCV_PIXELCHUNK_CXX

#include "PixelChunk.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {


  void
  PixelChunk::BasicFill(const GEO2D_Contour_t& contour)
  {
    Reset();

    // Set the contour
    ctor   = contour;

    // Find the edges
    larocv::FindEdges(ctor,edge1,edge2);

    // Rotated rect coordinates
    auto min_rect  = MinAreaRect(ctor);
    cv::Point2f vertices[4];
    min_rect.points(vertices);
    
    // Set parameters from rotated rect
    auto rect  = min_rect.size;
    length     = rect.height > rect.width ? rect.height : rect.width;
    width      = rect.height > rect.width ? rect.width  : rect.height;
    perimeter  = ArcLength(ctor);
    area       = ContourArea(ctor);
    ctorPCA    = CalcPCA(ctor);
    angular_sum= CircumferenceAngularSum(ctor);
  }

  void
  PixelChunk::BasicImageFill(const GEO2D_Contour_t& contour, cv::Mat& adc_img, float threshold) {
    BasicFill(contour);

    auto mask_img = larocv::MaskImage(adc_img,contour,0,false);

    if (threshold > 0.0)
      mask_img = Threshold(mask_img,threshold,255);

    npixel   = CountNonZero(mask_img);
    if(!npixel) throw larbys("Given contour contains no pixels");
    pixelPCA = larocv::CalcPCA(mask_img);
    mean_pixel_dist  = larocv::MeanDistanceToLine (mask_img,pixelPCA);
    sigma_pixel_dist = larocv::SigmaDistanceToLine(mask_img,pixelPCA);
  }


  void
  PixelChunk::AdvancedImageFill(const GEO2D_Contour_t& contour, cv::Mat& adc_img, cv::Mat& trk_img, cv::Mat& shr_img, float threshold) {
    BasicImageFill(contour,adc_img,threshold);
    auto thresh_trk_img = larocv::MaskImage(trk_img,contour,0,false);
    auto thresh_shr_img = larocv::MaskImage(shr_img,contour,0,false);

    if (threshold>0.0) {
      thresh_trk_img = Threshold(thresh_trk_img,threshold,255);
      thresh_shr_img = Threshold(thresh_shr_img,threshold,255);
    }

    track_frac = (float) CountNonZero(thresh_trk_img) / (float) npixel;
    shower_frac= (float) CountNonZero(thresh_shr_img) / (float) npixel;
  }

}
#endif
