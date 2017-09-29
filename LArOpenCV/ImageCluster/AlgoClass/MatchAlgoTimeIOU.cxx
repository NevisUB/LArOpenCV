#ifndef __MATCHALGOTIMEIOU_CXX__
#define __MATCHALGOTIMEIOU_CXX__

#include "MatchAlgoTimeIOU.h"

namespace larocv {

  void MatchAlgoTimeIOU::Configure(const Config_t &pset) {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
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

    float timeiou;

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

    // get the contour
    const auto& ctor0 = par0._ctor;
    const auto& ctor1 = par1._ctor;
    const auto& ctor2 = par2._ctor;

    float timeiou;
    
    score = timeiou;
    return score;
  }

}


#endif
