#ifndef __MATCHALGOBASE_H__
#define __MATCHALGOBASE_H__

#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "LArOpenCV/Core/ImageMeta.h"
#include "LArOpenCV/ImageCluster/Base/BaseUtil.h"
#include "LArOpenCV/ImageCluster/Base/MatchBookKeeper.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include <vector>
#include <array>

namespace larocv {

  class MatchAlgoBase : public laropencv_base {

  public:

    MatchAlgoBase(const std::string name) : laropencv_base(name) {}
    MatchAlgoBase() : laropencv_base("MatchAlgoBase") {}

    virtual ~MatchAlgoBase() {}
    
    virtual void
    Configure(const Config_t &pset) = 0;

    virtual void
    Initialize() = 0;

    virtual float
    Match(const cv::Mat& img0, 
          const cv::Mat& img1,
	  const ImageMeta& meta0,
	  const ImageMeta& meta1,
	  const data::ParticleCluster& par0, 
	  const data::ParticleCluster& par1) = 0;

    virtual float
    Match(const cv::Mat& img0, 
	  const cv::Mat& img1, 
	  const cv::Mat& img2,
	  const ImageMeta& meta0,
	  const ImageMeta& meta1,
	  const ImageMeta& meta2,
	  const data::ParticleCluster& par0,
	  const data::ParticleCluster& par1,
	  const data::ParticleCluster& par2) = 0;

    std::vector<std::vector<std::pair<size_t,size_t> > >
    Match();
    
    void ClearMatch();
    void ClearEvent();

    void Register(const data::ParticleCluster& pc,size_t plane);
    void Register(const cv::Mat& img, size_t plane);
    void Register(const ImageMeta& meta, size_t plane);

    const data::ParticleCluster* Particle(size_t plane,size_t id);

  protected:

    std::array<const cv::Mat*,3>   _img_v;
    std::array<const ImageMeta*,3> _meta_v;
    
  private:
    
    MatchBookKeeper _MatchBookKeeper;
    std::vector<size_t> _seed_v;

    std::array<std::vector<size_t>,3> _clusters_per_plane_vv;
    std::vector<const data::ParticleCluster*> _particle_ptr_v;
    std::vector<std::pair<size_t,size_t> > _particle_id_to_plane_v;

    std::vector<uint> _match_v;
    
    float _threshold;

    std::array<std::vector<const data::ParticleCluster*>,3> _pcluster_vv;

  };

}

#endif
