#ifndef __LINEARITYANALYSIS_CXX__
#define __LINEARITYANALYSIS_CXX__

#include "LinearityAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {
  
  static LinearityAnalysisFactory __global_LinearityAnalysisFactory__;
  
  void LinearityAnalysis::_Configure_(const Config_t &pset)
  {

    auto name_combined = pset.get<std::string>("VertexProducer");
    _combined_id=kINVALID_ALGO_ID;
    if (!name_combined.empty()) {
      _combined_id = this->ID(name_combined);
      if (_combined_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    auto name_particle = pset.get<std::string>("ParticleProducer");
    _particle_id=kINVALID_ALGO_ID;
    if (!name_particle.empty()) {
      _particle_id = this->ID(name_particle);
      if (_particle_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Seed ID algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    _tree = new TTree("LinearityAnalysis","");
    AttachIDs(_tree);
    _tree->Branch("roid" , &_roid  , "roid/I");
    _tree->Branch("vtxid", &_vtxid , "vtxid/I");
    _tree->Branch("x"    , &_x     , "x/D");
    _tree->Branch("y"    , &_y     , "y/D");
    _tree->Branch("z"    , &_z     , "z/D");


    
    _roid = 0;
  }
  
  void LinearityAnalysis::_Process_() {
    
    if(NextEvent()) _roid=0;

    auto& ass_man = AssManager();
    
    // Get images
    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto thresh_img_v = adc_img_v;
    for(auto& img : thresh_img_v)
      img = Threshold(img,10,255);

    const auto& meta_v = MetaArray();
    
    const auto& vtx3d_arr = AlgoData<data::Vertex3DArray>(_combined_id,0);
    const auto& vtx3d_v = vtx3d_arr.as_vector();
    
    const auto& particle_arr = AlgoData<data::ParticleArray>(_particle_id,0);
    const auto& particle_v = particle_arr.as_vector();

    _vtxid = -1;
    LAROCV_DEBUG() << "Got " << vtx3d_v.size() << " vertices" << std::endl;
    for(size_t vtxid = 0; vtxid < vtx3d_v.size(); ++vtxid) {
      const auto& vtx3d = vtx3d_v[vtxid];
      
      auto par_id_v = ass_man.GetManyAss(vtx3d,particle_arr.ID());
      if (par_id_v.empty()) continue;

      _vtxid += 1;

      _x = vtx3d.x;
      _y = vtx3d.y;
      _z = vtx3d.z;
      
      
      
      
      
      
      
      _tree->Fill();
    } // end this vertex
    
    _roid += 1;
  }
}
#endif
