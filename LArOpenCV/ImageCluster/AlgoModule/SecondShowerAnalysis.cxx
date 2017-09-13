#ifndef __SECONDSHOWERANALYSIS_CXX__
#define __SECONDSHOWERANALYSIS_CXX__

#include "SecondShowerAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"

namespace larocv {

  static SecondShowerAnalysisFactory __global_SecondShowerAnalysisFactory__;

  void SecondShowerAnalysis::_Configure_(const Config_t &pset)
  {

    auto name_combined = pset.get<std::string>("VertexProducer");
    _combined_id=kINVALID_ALGO_ID;
    if (!name_combined.empty()) {
      _combined_id = this->ID(name_combined);
      if (_combined_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "VertexProducer ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    auto name_particle = pset.get<std::string>("ParticleProducer");
    _particle_id=kINVALID_ALGO_ID;
    if (!name_particle.empty()) {
      _particle_id = this->ID(name_particle);
      if (_particle_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "ParticleProducer ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    _tree = new TTree("SecondShowerAnalysis","");
    _tree->Branch("x" , &_x , "x/D");
    _tree->Branch("y" , &_y , "y/D");
    _tree->Branch("z" , &_z , "z/D");

    _roid = 0;
  }

  void SecondShowerAnalysis::_Process_() {
    
    LAROCV_INFO() << "start" << std::endl;

    if(NextEvent()) _roid=0;

    auto& ass_man = AssManager();
    
    // Get images
    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto thresh_img_v = adc_img_v;
    for(auto& img : thresh_img_v)
      img = Threshold(img,10,255);

    const auto& meta_v = MetaArray();

    // Get the particle clusters from the previous module, go vertex-by-vertex
    const auto& vtx3d_arr = AlgoData<data::Vertex3DArray>(_combined_id,0);
    const auto& vtx3d_v = vtx3d_arr.as_vector();
    
    const auto& particle_arr = AlgoData<data::ParticleArray>(_particle_id,0);
    const auto& particle_v = particle_arr.as_vector();
    
    _vtxid = -1;

    LAROCV_DEBUG() << "Got " << vtx3d_v.size() << " vertices" << std::endl;
    for(size_t vtxid = 0; vtxid < vtx3d_v.size(); ++vtxid) {
      ClearVertex();

      const auto& vtx3d = vtx3d_v[vtxid];
      
      auto par_id_v = ass_man.GetManyAss(vtx3d,particle_arr.ID());
      if (par_id_v.empty()) continue;

      _vtxid += 1;
      
      _x = vtx3d.x;
      _y = vtx3d.y;
      _z = vtx3d.z;
      
      LAROCV_DEBUG() << "Got " << par_id_v.size() << " particles" << std::endl;
      LAROCV_DEBUG() << " & algo data particle vector sz " << particle_v.size() << std::endl;

      ResetVectors(par_id_v.size());
      
      for(size_t par_idx=0; par_idx<par_id_v.size(); ++par_idx) {

	auto par_id = par_id_v[par_idx];
	const auto& par = particle_v[par_id];
	
	for(size_t plane=0; plane<3; ++plane) { }
      }
      _tree->Fill();
    } // end this vertex
    
    _roid += 1;
    LAROCV_INFO() << "end" << std::endl;
  }

  void SecondShowerAnalysis::ResetVectors(size_t sz) {
      
    return;
  }

  void SecondShowerAnalysis::ClearVertex() {
    _x = kINVALID_DOUBLE;
    _y = kINVALID_DOUBLE;
    _z = kINVALID_DOUBLE;

  }

}
#endif
