#ifndef __VERTEXSEEDRESIZE_CXX__
#define __VERTEXSEEDRESIZE_CXX__

#include "VertexSeedResize.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {

  /// Global VertexSeedResizeFactory to register AlgoFactory
  static VertexSeedResizeFactory __global_VertexSeedResizeFactory__;
  
  void VertexSeedResize::_Configure_(const Config_t &pset)
  {
    auto name_seed = pset.get<std::string>("VertexSeedName");

    if (!name_seed.empty()) {
      _vertexseed_id      = this->ID(name_seed);
      if (_vertexseed_id == kINVALID_ALGO_ID) {
        LAROCV_CRITICAL() << "VertexSeed algorithm name does not exist!" << std::endl;
        throw larbys();
      }
    }

    _slogger_level = logger::get_shared().level();
    
    _thresh   = pset.get<float>("DensityThreshold",0.7);
    _start_rad= pset.get<float>("RadiusStart",3);
    _end_rad  = pset.get<float>("RadiusEnd",15);
    _step     = pset.get<float>("RadiusStep",1);

    Register(new data::VertexSeed3DArray);
  }
  
  void VertexSeedResize::Finalize(TFile*) { }

  bool VertexSeedResize::_PostProcess_() const
  { return true; }
  
  void VertexSeedResize::_Process_()
  {

    auto img_v = ImageArray();

    const auto& seed_array = AlgoData<data::VertexSeed3DArray>(_vertexseed_id,0);
    const auto& seed_v = seed_array.as_vector();
    
    auto& data = AlgoData<data::VertexSeed3DArray>(0);
    
    // For each seed, estimate the best radius to use from the ADC image
    for(const auto& seed : seed_v) {
      auto vtx3d = seed;
      LAROCV_DEBUG() << "Inspected vertex seed @ (" << vtx3d.x << "," << vtx3d.y << "," << vtx3d.z << ")" << std::endl;
      for(size_t plane=0; plane < 3; ++plane) {
	const auto& img = img_v.at(plane);
	auto& vtx2d = vtx3d.vtx2d_v.at(plane);
	geo2d::Circle<float> res;
	LAROCV_DEBUG() << "... @ plane " << plane << " 2d seed (" << vtx2d.x << "," << vtx2d.y << ")" << std::endl;
	logger::get_shared().set(this->logger().level());
	auto found = ChargeBlobCircleEstimate(img,       // image
					      vtx2d,     // 2d center
					      res,       // result circle
					      _thresh,   // density threshold
					      _start_rad,// start radius
					      _end_rad,  // end radius
					      _step);    // step
	logger::get_shared().set(_slogger_level);
	LAROCV_DEBUG() << "Found " << found << std::endl;
	if(!found) continue;

	LAROCV_DEBUG() << " radius " << res.radius << std::endl;
	vtx2d = data::VertexSeed2D(res);
      }
      data.emplace_back(std::move(vtx3d));
    }
    
  }
  
}
#endif
