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
    
    _threshold = pset.get<float>("Threshold");
    auto name_seed = pset.get<std::string>("EdgeSeedProducer","");
    _seed_id=kINVALID_ALGO_ID;
    if (!name_seed.empty()) {
      _seed_id = this->ID(name_seed);
      if (_seed_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    for(short plane=0;plane<3;++plane)
      Register(new data::VertexSeed2DArray);
  }
  
  void ShowerPlaneSeeds::_Process_(const larocv::Cluster2DArray_t& clusters,
				   ::cv::Mat& img,
				   larocv::ImageMeta& meta,
				   larocv::ROI& roi)
    
  {
    _OneTrackOneShower.SetPlaneInfo(meta);

    auto& vertexseed2darray = AlgoData<data::VertexSeed2DArray>(meta.plane());


    if (_seed_id == kINVALID_ALGO_ID) {
      LAROCV_DEBUG() << "No input seed given, determine track edges" << std::endl;
      auto thresh_img = Threshold(img,_threshold,255);
      auto ctor_v     = FindContours(thresh_img);
    
      for(const auto& ctor : ctor_v) {
	geo2d::Vector<float> edge1(kINVALID_FLOAT,kINVALID_FLOAT);
	auto edge2 = edge1;
	
	FindEdges(ctor,edge1,edge2);

	if (edge1.x!=kINVALID_FLOAT && edge1.y!=kINVALID_FLOAT)
	  vertexseed2darray.emplace_back(std::move(edge1));

	if (edge1.x!=kINVALID_FLOAT && edge1.y!=kINVALID_FLOAT)
	  vertexseed2darray.emplace_back(std::move(edge2));
	
      }
      LAROCV_DEBUG() << "Saved " << vertexseed2darray.as_vector().size() << " 2D seeds" << std::endl;
      return;
    } 

    LAROCV_DEBUG() << "Track input seeds given, filtering on shower image" << std::endl;

    const auto& seed_data_arr = AlgoData<data::VertexSeed2DArray>(_seed_id,meta.plane());
    const auto& seed_data_v   = seed_data_arr.as_vector();

    LAROCV_DEBUG() << "Got " << seed_data_v.size() << " 2D track edge seeds" << std::endl;
    
    uint ix=0;
    
    for(const auto& seed_ : seed_data_v) {
      auto seed = seed_;
      data::CircleVertex cvtx;
      cvtx.center.x = seed.x;
      cvtx.center.y = seed.y;
      cvtx.radius   = _OneTrackOneShower.circle_default_size();
      seed.radius   = cvtx.radius;
      
      _OneTrackOneShower.ValidateCircleVertex(img,cvtx);
      LAROCV_DEBUG() << ix << ") @ (" << seed.x << "," << seed.y << ") xs ..." << cvtx.xs_v.size() << std::endl;
      ix++;

      if (cvtx.xs_v.size()!=1) continue;

      LAROCV_DEBUG() << "... saved" << std::endl;
      
      vertexseed2darray.emplace_back(std::move(seed));
    }

    return;
  }
}
#endif
