#ifndef __MATCHALGOTIMEIOU_CXX__
#define __MATCHALGOTIMEIOU_CXX__

#include "MatchAlgoTimeIOU.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {

  void MatchAlgoTimeIOU::Configure(const Config_t &pset) {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    _threshold = pset.get<float>("Threshold",0.1);
  }
    
  void MatchAlgoTimeIOU::Initialize() { }


  float MatchAlgoTimeIOU::Match(const cv::Mat& img0, 
				const cv::Mat& img1,
				const ImageMeta& meta0,
				const ImageMeta& meta1,
				const data::ParticleCluster& par0, 
				const data::ParticleCluster& par1) {

    float score = -1.0 * kINVALID_FLOAT;

    const auto& ctor0 = par0._ctor;
    const auto& ctor1 = par1._ctor;

    auto ctor_img0 = MaskImage(img0,ctor0,0,false);
    auto ctor_img1 = MaskImage(img1,ctor1,0,false);

    auto nzero0_v = FindNonZero(ctor_img0);
    auto nzero1_v = FindNonZero(ctor_img1);

    //min @ max time tick
    int min_time_tick0 = kINVALID_INT;
    int max_time_tick0 = -1.0*kINVALID_INT;

    for(const auto& px : nzero0_v) {
      if (px.x < min_time_tick0) min_time_tick0 = px.x;
      if (px.x > max_time_tick0) max_time_tick0 = px.x;
    }

    int min_time_tick1 = kINVALID_INT;
    int max_time_tick1 = -1.0*kINVALID_INT;

    for(const auto& px : nzero1_v) {
      if (px.x < min_time_tick1) min_time_tick1 = px.x;
      if (px.x > max_time_tick1) max_time_tick1 = px.x;
    }

    auto min_time0 = min_time_tick0;//meta0.XtoTimeTick(min_time_tick0);
    auto min_time1 = min_time_tick1;//meta1.XtoTimeTick(min_time_tick1);

    auto max_time0 = max_time_tick0;//meta0.XtoTimeTick(max_time_tick0);
    auto max_time1 = max_time_tick1;//meta1.XtoTimeTick(max_time_tick1);

    //which one is higher in time?
    float max_time_top = kINVALID_FLOAT;
    float min_time_top = kINVALID_FLOAT;

    float max_time_bot = kINVALID_FLOAT;
    float min_time_bot = kINVALID_FLOAT;

    if (max_time0 > max_time1) {
      max_time_top = max_time0;
      min_time_top = min_time0;

      max_time_bot = max_time1;
      min_time_bot = min_time1;
    } else {
      max_time_top = max_time1;
      min_time_top = min_time1;

      max_time_bot = max_time0;
      min_time_bot = min_time0;
    }
    
    LAROCV_DEBUG() << "top (min,max)=(" << min_time_top << "," << max_time_top << ")" << std::endl;
    LAROCV_DEBUG() << "bot (min,max)=(" << min_time_bot << "," << max_time_bot << ")" << std::endl;

    if (min_time_top > max_time_bot) return score;

    auto max_time = max_time_top;
    auto min_time = std::min(min_time_top,min_time_bot);

    auto com_max_time = max_time_bot;
    auto com_min_time = std::max(min_time_top,min_time_bot);
    
    float timeiou = (com_max_time - com_min_time)  / (max_time - min_time);
    
    score = timeiou;
    return score;
  }
    
    
  float MatchAlgoTimeIOU::Match(const cv::Mat& img0, 
				const cv::Mat& img1, 
				const cv::Mat& img2,
				const ImageMeta& meta0,
				const ImageMeta& meta1,
				const ImageMeta& meta2,
				const data::ParticleCluster& par0,
				const data::ParticleCluster& par1,
				const data::ParticleCluster& par2) {

    float score = -1.0 * kINVALID_FLOAT;
    return score;
  }

}


#endif
