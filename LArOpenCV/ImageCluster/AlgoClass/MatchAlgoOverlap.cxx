#ifndef __MATCHALGOOVERLAP_CXX__
#define __MATCHALGOOVERLAP_CXX__

#include "MatchAlgoOverlap.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include <cassert>

namespace larocv {

  void MatchAlgoOverlap::Configure(const Config_t &pset) {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    _match_weight_by_size = pset.get<bool>("MatchWeightBySize",false);
    _threshold = pset.get<float>("Threshold",0.1);
  }
    
  void MatchAlgoOverlap::Initialize() {
    for(size_t plane=0; plane<3; ++plane) { 
      assert(_meta_v.at(plane));
      _geo.ResetPlaneInfo(*(_meta_v.at(plane)));
    }
  }

  float MatchAlgoOverlap::Match(const cv::Mat& img0, 
				const cv::Mat& img1,
				const ImageMeta& meta0,
				const ImageMeta& meta1,
				const data::ParticleCluster& par0, 
				const data::ParticleCluster& par1) {

    float score = -1.0 * kINVALID_FLOAT;
    
    const auto& ctor0 = par0._ctor;
    const auto& ctor1 = par1._ctor;

    if (ctor0.empty()) throw larbys();
    if (ctor1.empty()) throw larbys();

    LAROCV_DEBUG() << "ctor size "
		   << " @ 0: " << ctor0.size()
		   << " @ 1: " << ctor1.size() << std::endl;

    auto ctor_img0 = MaskImage(img0,ctor0,0,false);
    auto ctor_img1 = MaskImage(img1,ctor1,0,false);

    auto nzero0_i = FindNonZero(ctor_img0);
    auto nzero1_i = FindNonZero(ctor_img1);

    LAROCV_DEBUG() << "non zero pts "
		   << " @ 0: " << nzero0_i.size()
		   << " @ 1: " << nzero1_i.size() << std::endl;
	
    // cast to float
    geo2d::VectorArray<float> nzero0_f;
    nzero0_f.reserve(nzero0_i.size());

    geo2d::VectorArray<float> nzero1_f;
    nzero1_f.reserve(nzero1_i.size());
	
    for(const auto& pt : nzero0_i)
      nzero0_f.emplace_back(pt.x,pt.y);
	
    for(const auto& pt : nzero1_i)
      nzero1_f.emplace_back(pt.x,pt.y);
	
    auto overlap = Overlap(nzero0_f,meta0.plane(),nzero1_f,meta1.plane(),false);

    if (_match_weight_by_size) {
      float num   = std::min(nzero0_f.size(),nzero1_f.size());
      float denom = std::max(nzero0_f.size(),nzero1_f.size());

      if (denom == 0) return score;
	  
      overlap *= num/denom;
    }	
	
    score = overlap;
    LAROCV_DEBUG() << "ret=" << score << std::endl;
    return score;
  }
    
    
  float MatchAlgoOverlap::Match(const cv::Mat& img0, 
				const cv::Mat& img1, 
				const cv::Mat& img2,
				const ImageMeta& meta0,
				const ImageMeta& meta1,
				const ImageMeta& meta2,
				const data::ParticleCluster& par0,
				const data::ParticleCluster& par1,
				const data::ParticleCluster& par2) {

    float score = -1.0 * kINVALID_FLOAT;

    LAROCV_DEBUG() << "... accepted" << std::endl;
	
    // get the contour
    const auto& ctor0 = par0._ctor;
    const auto& ctor1 = par1._ctor;
    const auto& ctor2 = par2._ctor;

    auto ctor_img0 = MaskImage(img0,ctor0,0,false);
    auto ctor_img1 = MaskImage(img1,ctor1,0,false);
    auto ctor_img2 = MaskImage(img2,ctor2,0,false);
	
    auto nzero0_i = FindNonZero(ctor_img0);
    auto nzero1_i = FindNonZero(ctor_img1);
    auto nzero2_i = FindNonZero(ctor_img2);

    //cast to float
    geo2d::VectorArray<float> nzero0_f;
    nzero0_f.reserve(nzero0_i.size());

    geo2d::VectorArray<float> nzero1_f;
    nzero1_f.reserve(nzero1_i.size());

    geo2d::VectorArray<float> nzero2_f;
    nzero2_f.reserve(nzero2_i.size());
	
    for(const auto& pt : nzero0_i)
      nzero0_f.emplace_back(pt.x,pt.y);

    for(const auto& pt : nzero1_i)
      nzero1_f.emplace_back(pt.x,pt.y);

    for(const auto& pt : nzero2_i)
      nzero2_f.emplace_back(pt.x,pt.y);
	
    auto overlap01 = Overlap(nzero0_f,meta0.plane(),nzero1_f,meta1.plane(),false);
    auto overlap02 = Overlap(nzero0_f,meta0.plane(),nzero2_f,meta2.plane(),false);
    auto overlap12 = Overlap(nzero1_f,meta1.plane(),nzero2_f,meta2.plane(),false);

    if (overlap01 == 0.0) return score;
    if (overlap02 == 0.0) return score;
    if (overlap12 == 0.0) return score;

    if (_match_weight_by_size) {
	  
      float num01   = std::min(nzero0_f.size(),nzero1_f.size());
      float num02   = std::min(nzero0_f.size(),nzero2_f.size());
      float num12   = std::min(nzero1_f.size(),nzero2_f.size());
	  
      float denom01 = std::max(nzero0_f.size(),nzero1_f.size());
      float denom02 = std::max(nzero0_f.size(),nzero2_f.size());
      float denom12 = std::max(nzero1_f.size(),nzero2_f.size());

	  
      if (denom01 > 0) overlap01 *= num01/denom01;
      else overlap01 = 0.0;

      if (denom02 > 0) overlap02 *= num02/denom02;
      else overlap02 = 0.0;
	  
      if (denom12 > 0) overlap12 *= num12/denom12;
      else overlap12 = 0.0;
			  
    }
	

    auto overlap = overlap01 + overlap02 + overlap12;
    overlap /= 3.0;

    score = overlap;
    LAROCV_DEBUG() << "ret=" << score << std::endl;
    return score;
  }


  float MatchAlgoOverlap::Overlap(const geo2d::VectorArray<float>& pts0_v, const size_t plane0,
				  const geo2d::VectorArray<float>& pts1_v, const size_t plane1,
				  bool overcover) const
  {

    if (pts0_v.empty()) return 0.0;
    if (pts1_v.empty()) return 0.0;
    
    // Get the smallest set of points
    const geo2d::VectorArray<float>* max_pts_v=nullptr;
    const geo2d::VectorArray<float>* min_pts_v=nullptr;

    size_t min_plane=kINVALID_SIZE;
    size_t max_plane=kINVALID_SIZE;
    
    if (pts0_v.size() >= pts1_v.size()) {
      max_pts_v = &pts0_v;
      min_pts_v = &pts1_v;
      max_plane = plane0;
      min_plane = plane1;
    }
    else {
      max_pts_v = &pts1_v;
      min_pts_v = &pts0_v;
      max_plane = plane1;
      min_plane = plane0;
    }

    float overlap=0.0;
    std::vector<bool> used_v(max_pts_v->size(),false);
    for(size_t min_id=0;min_id<min_pts_v->size();++min_id) {
      const auto& min_pt = (*min_pts_v)[min_id];
      for(size_t max_id=0;max_id<max_pts_v->size();++max_id) {
	if (used_v[max_id]) continue;
	const auto& max_pt = (*max_pts_v)[max_id];
	if (_geo.YZPoint(min_pt,min_plane,max_pt,max_plane)) {
	  overlap+=1.0;
	  used_v[max_id] = true;
	  if (!overcover) break;
	}
      }
    }

    // it's possible for overlap to be greater than 1...
    // a point on the first plane may overlap 2 points on the second?
    
    overlap /= (float)min_pts_v->size();
    
    //LAROCV_DEBUG() << "Calculated overlap " << overlap << std::endl;
    return overlap;
  }

}


#endif
