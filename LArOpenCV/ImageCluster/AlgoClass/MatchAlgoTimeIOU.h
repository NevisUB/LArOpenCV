#ifndef __MATCHALGOTIMEIOU_H__
#define __MATCHALGOTIMEIOU_H__

#include "LArOpenCV/ImageCluster/AlgoClass/MatchAlgoBase.h"

namespace larocv {

  class MatchAlgoTimeIOU : public MatchAlgoBase {

  public:

     MatchAlgoTimeIOU() : MatchAlgoBase("MathAlgoTimeIOU") {}
    ~MatchAlgoTimeIOU() {}
    
    void Configure(const Config_t &pset);

    void Initialize();
    
    float Match(const cv::Mat& img0, 
		const cv::Mat& img1,
		const ImageMeta& meta0,
		const ImageMeta& meta1,
		const data::ParticleCluster& par0, 
		const data::ParticleCluster& par1);
    
    float Match(const cv::Mat& img0, 
		const cv::Mat& img1, 
		const cv::Mat& img2,
		const ImageMeta& meta0,
		const ImageMeta& meta1,
		const ImageMeta& meta2,
		const data::ParticleCluster& par0,
		const data::ParticleCluster& par1,
		const data::ParticleCluster& par2);
    
  protected:

    std::array<const cv::Mat*,3>   _img_v;
    std::array<const ImageMeta*,3> _meta_v;
    
  private:

  };
  
}

#endif
