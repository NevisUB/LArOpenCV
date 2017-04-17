#ifndef __SHAPEANALYSIS_CXX__
#define __SHAPEANALYSIS_CXX__

#include "ShapeAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PixelChunk.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

namespace larocv {

  /// Global larocv::ShapeAnalysisFactory to register AlgoFactory
  static ShapeAnalysisFactory __global_ShapeAnalysisFactory__;
  
  void ShapeAnalysis::_Configure_(const Config_t &pset)
  {

    auto name_combined = pset.get<std::string>("CombinedProducer");
    _combined_id=kINVALID_ALGO_ID;
    if (!name_combined.empty()) {
      _combined_id = this->ID(name_combined);
      if (_combined_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }


    Register(new data::Vertex3DArray);
    for(short plane=0;plane<3;++plane) Register(new data::ParticleClusterArray);
    for(short plane=0;plane<3;++plane) Register(new data::TrackClusterCompoundArray);
  }
  
  void ShapeAnalysis::_Process_()
  {

    auto& ass_man = AssManager();
    
    // Get images
    auto adc_img_v     = ImageArray(ImageSetID_t::kImageSetWire);
    auto track_img_v   = ImageArray(ImageSetID_t::kImageSetTrack);
    auto shower_img_v  = ImageArray(ImageSetID_t::kImageSetShower);

    // Get the particle clusters from the previous module, go vertex-by-vertex
    const auto& combined_vtx_v = AlgoData<data::Vertex3DArray>(_combined_id,0).as_vector();
    for(const auto& combined_vtx : combined_vtx_v) {

      for(size_t plane=0;plane<3;++plane) {
	const auto& combined_par_v = AlgoData<data::ParticleClusterArray>(_combined_id,plane);
	auto par_ass_id_v = ass_man.GetManyAss(combined_vtx,combined_par_v.ID());
	
	for(auto par_ass_id : par_ass_id_v) {
	  const auto& par = combined_par_v.as_vector().at(par_ass_id);

	  auto& adc_img    = adc_img_v.at(plane);
	  auto& track_img  = track_img_v.at(plane);
	  auto& shower_img = shower_img_v.at(plane);
	  
	  PixelChunk pchunk(par._ctor,adc_img,track_img,shower_img);
	  
	  //Do something with this pixel chunk, make atomics?
	  
	}
      }
    }
    
  }
}
#endif
