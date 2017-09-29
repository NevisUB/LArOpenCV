#ifndef __MATCHALGOOVERLAP_H__
#define __MATCHALGOOVERLAP_H__

#include "LArOpenCV/ImageCluster/AlgoClass/MatchAlgoBase.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"

namespace larocv {

  class MatchAlgoOverlap : public MatchAlgoBase {

  public:

     MatchAlgoOverlap() : MatchAlgoBase("MatchAlgoBase") {}
    ~MatchAlgoOverlap() {}
    
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

    float Overlap(const geo2d::VectorArray<float>& pts0_v, const size_t plane0,
		  const geo2d::VectorArray<float>& pts1_v, const size_t plane1,
		  bool overcover) const;
    
  protected:

    std::array<const cv::Mat*,3>   _img_v;
    std::array<const ImageMeta*,3> _meta_v;
    
  private:
    
    LArPlaneGeo _geo;
    bool _match_weight_by_size;

  };
  
}

#endif
