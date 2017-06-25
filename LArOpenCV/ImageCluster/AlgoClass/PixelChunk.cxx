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
    auto rect  =  min_rect.size;
    length     =  rect.height > rect.width ? rect.height : rect.width;
    width      =  rect.height > rect.width ? rect.width  : rect.height;
    perimeter  =  ArcLength(ctor);
    area        = ContourArea(ctor);
    try {
      ctorPCA     = CalcPCA(ctor);
    }
    catch (const larbys& larerr) {}

    angular_sum = CircumferenceAngularSum(ctor);
  }

  void
  PixelChunk::BasicImageFill(const GEO2D_Contour_t& contour, cv::Mat& adc_img, float threshold) {
    BasicFill(contour);

    auto mask_img = MaskImage(adc_img,contour,0,false);

    if (threshold > 0.0)
      mask_img = Threshold(mask_img,threshold,255);

    npixel   = CountNonZero(mask_img);

    try {
      pixelPCA = CalcPCA(mask_img);
      mean_pixel_dist  = MeanDistanceToLine (mask_img,pixelPCA);
      sigma_pixel_dist = SigmaDistanceToLine(mask_img,pixelPCA);
    }
    catch (const larbys& larerr) {}

  }


  void
  PixelChunk::AdvancedImageFill(const GEO2D_Contour_t& contour, cv::Mat& adc_img, cv::Mat& trk_img, cv::Mat& shr_img, float threshold) {
    BasicImageFill(contour,adc_img,threshold);
    auto thresh_trk_img = MaskImage(trk_img,contour,0,false);
    auto thresh_shr_img = MaskImage(shr_img,contour,0,false);
    if (threshold>0.0) {
      thresh_trk_img = Threshold(thresh_trk_img,threshold,255);
      thresh_shr_img = Threshold(thresh_shr_img,threshold,255);
    }

    if (npixel) {
      track_frac = (float) CountNonZero(thresh_trk_img) / (float) npixel;
      shower_frac= (float) CountNonZero(thresh_shr_img) / (float) npixel;
    }
    else {
      track_frac = 0.0;
      shower_frac= 0.0;
    }

  }
  
  
  
  bool
  PixelChunk::EstimateStartEndPixel(const geo2d::Vector<float>& vertex,cv::Mat& adc_img, float threshold) {
    if (ctor.empty()) return false;

    auto mask_img = MaskImage(adc_img,ctor,0,false);
    
    if (threshold > 0.0)
      mask_img = Threshold(mask_img,threshold,255);
    
    auto pts_v = FindNonZero(mask_img);

    float min_dist = kINVALID_FLOAT;
    float max_dist = -1.0*kINVALID_FLOAT;
    size_t min_id = kINVALID_SIZE;
    size_t max_id = kINVALID_SIZE;

    for(unsigned pt_id=0; pt_id < pts_v.size(); ++pt_id) {
      auto dist = geo2d::dist(vertex,geo2d::Vector<float>(pts_v[pt_id]));
      if (dist > max_dist){
	max_dist = dist;
	max_id = pt_id;
      }
      if (dist < min_dist) {
	min_dist = dist;
	min_id = pt_id;
      }
    }

    if (min_id == kINVALID_SIZE) return false;
    if (max_id == kINVALID_SIZE) return false;
	
    start_pt = geo2d::Vector<float>(pts_v.at(min_id));
    end_pt = geo2d::Vector<float>(pts_v.at(max_id));
    
    return true;
  }


  bool
  PixelChunk::EstimateTrunkPCA(const geo2d::Vector<float>& vertex, cv::Mat& adc_img, float distance, float threshold) {
    if (ctor.empty()) return false;

    auto mask_img = MaskImage(adc_img,ctor,0,false);
    
    if (threshold > 0.0)
      mask_img = Threshold(mask_img,threshold,255);
    
    mask_img = MaskImage(mask_img,geo2d::Circle<float>(vertex,distance),0,false);
    
    try {
      trunkPCA = CalcPCA(mask_img);
    }
    catch (const larbys& larerr) { return false; }

    return true;
  }
  
}
#endif
