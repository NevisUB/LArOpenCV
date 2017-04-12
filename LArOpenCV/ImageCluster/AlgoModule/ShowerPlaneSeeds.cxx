#ifndef __SHOWERPLANESEEDS_CXX__
#define __SHOWERPLANESEEDS_CXX__

#include "ShowerPlaneSeeds.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"

namespace larocv {

  /// Global larocv::ShowerPlaneSeedsFactory to register AlgoFactory
  static ShowerPlaneSeedsFactory __global_ShowerPlaneSeedsFactory__;
  
  void ShowerPlaneSeeds::_Configure_(const Config_t &pset)
  {
    
    _OneTrackOneShower.set_verbosity(this->logger().level());
    _OneTrackOneShower.Configure(pset.get<Config_t>("OneTrackOneShower"));
    
    for(short plane=0;plane<3;++plane)
      Register(new data::VertexSeed2DArray);
  }
  
  void ShowerPlaneSeeds::_Process_()
  {

    auto track_img_v   = ImageArray(ImageSetID_t::kImageSetTrack);
    auto shower_img_v  = ImageArray(ImageSetID_t::kImageSetShower);

    assert(track_img_v.size() == shower_img_v.size());
    auto nplanes = track_img_v.size();
      
    auto meta_v = MetaArray();
    
    for(auto const& meta : meta_v)
      _OneTrackOneShower.SetPlaneInfo(meta);

    for(size_t img_idx=0; img_idx<nplanes; ++img_idx) {

      auto& vertexseed2darray = AlgoData<data::VertexSeed2DArray>(img_idx);

      auto& track_img  = track_img_v[img_idx];
      auto& shower_img = shower_img_v[img_idx];

      std::vector<data::VertexSeed2D> vertexseed_v;

      LAROCV_DEBUG() << "No input seed given, determine track edges" << std::endl;
      auto thresh_img = Threshold(track_img,_threshold,255);
      auto ctor_v     = FindContours(thresh_img);
	
      for(const auto& ctor : ctor_v) {
	geo2d::Vector<float> edge1(kINVALID_FLOAT,kINVALID_FLOAT);
	auto edge2 = edge1;
	  
	FindEdges(ctor,edge1,edge2);
	  
	if (edge1.x!=kINVALID_FLOAT && edge1.y!=kINVALID_FLOAT)
	  vertexseed_v.emplace_back(std::move(edge1));
	  
	if (edge1.x!=kINVALID_FLOAT && edge1.y!=kINVALID_FLOAT)
	  vertexseed_v.emplace_back(std::move(edge2));
	  
      }
      LAROCV_DEBUG() << "Saved " << vertexseed_v.size() << " 2D seeds" << std::endl;
      
      LAROCV_DEBUG() << "Track input seeds given, filtering on shower image" << std::endl;
      
      uint ix=0;
      
      for(const auto& seed_ : vertexseed_v) {
	auto seed = seed_;
	data::CircleVertex cvtx;
	cvtx.center.x = seed.x;
	cvtx.center.y = seed.y;
	cvtx.radius   = _OneTrackOneShower.circle_default_size();
	seed.radius   = cvtx.radius;

	_OneTrackOneShower.ValidateCircleVertex(shower_img,cvtx);
	LAROCV_DEBUG() << ix << ") @ (" << seed.x << "," << seed.y << ") xs ..." << cvtx.xs_v.size() << std::endl;
	ix++;
	
	if (cvtx.xs_v.size()!=1) continue;
	
	LAROCV_DEBUG() << "... saved" << std::endl;
	
	vertexseed2darray.emplace_back(std::move(seed));
      }// end seed filter
    }// end plane
    
  }
}
#endif
