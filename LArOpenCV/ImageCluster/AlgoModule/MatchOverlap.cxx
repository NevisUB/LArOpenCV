#ifndef __MATCHOVERLAP_CXX__
#define __MATCHOVERLAP_CXX__

#include "MatchOverlap.h"

namespace larocv {

  /// Global larocv::MatchOverlapFactory to register AlgoFactory
  static MatchOverlapFactory __global_MatchOverlapFactory__;
  
  void MatchOverlap::_Configure_(const Config_t &pset)
  {
    _vertex_algo_id = kINVALID_ALGO_ID;
    _particle_cluster_algo_id = kINVALID_ALGO_ID;    
    
    auto vertex_algo_name = pset.get<std::string>("VertexAlgo");
    if (!vertex_algo_name.empty()) {
      _vertex_algo_id = this->ID(vertex_algo_name);
      if (_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given Vertex name is INVALID!");
    }
    
    auto particle_cluster_algo_name = pset.get<std::string>("ParticleClusterAlgo");
    if (!particle_cluster_algo_name.empty()) {
      _particle_cluster_algo_id = this->ID(particle_cluster_algo_name);
      if (_particle_cluster_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ParticleCluster name is INVALID!");
    }
    
    _vertex_algo_offset           = kINVALID_SIZE;
    _particle_cluster_algo_offset = kINVALID_SIZE;
    
    _vertex_algo_offset           = pset.get<size_t>("VertexAlgoOffset");
    _particle_cluster_algo_offset = pset.get<size_t>("ParticleClusterAlgoOffset");

    auto match_name = pset.get<std::string>("MatchAlgoName");
    _alg = nullptr;
    if (match_name == "MatchAlgoOverlap") {
      _alg = new MatchAlgoOverlap();
      _alg->Configure(pset.get<Config_t>("MatchAlgoOverlap"));
    }
    if (!_alg) throw larbys("Could not find match algo name");

    Register(new data::ParticleArray);
  }
  
  bool MatchOverlap::_PostProcess_() const
  { return true; }

  void MatchOverlap::_Process_() {
    LAROCV_DEBUG() << "start" << std::endl;
    auto wire_img_v  = ImageArray(ImageSetID_t::kImageSetWire);
    auto meta_v      = MetaArray(ImageSetID_t::kImageSetWire);

    _alg->ClearEvent();

    for(size_t plane=0; plane<3; ++plane) {
      _alg->Register(wire_img_v[plane],plane);
      _alg->Register(meta_v[plane],plane);
    }

    _alg->Initialize();

    std::array<const larocv::data::ParticleCluster*, 3> match_pcluster_v;
		   
    auto& assman = AssManager();
    
    auto& particle_data = AlgoData<data::ParticleArray>(0);

    const auto& vtx3d_arr = AlgoData<data::Vertex3DArray>(_vertex_algo_id,_vertex_algo_offset);
    const auto& vtx3d_v = vtx3d_arr.as_vector();

    LAROCV_DEBUG() << "Matching " << vtx3d_v.size() << " vertices" << std::endl;
    for(const auto& vtx3d : vtx3d_v) {
      _alg->ClearMatch();
      for(size_t plane=0; plane<3; ++plane) {
	const auto& par_data = AlgoData<data::ParticleClusterArray>(_particle_cluster_algo_id,
								    _particle_cluster_algo_offset+plane);
	const auto& par_v = par_data.as_vector();

	auto par_ass_id_v = assman.GetManyAss(vtx3d,par_data.ID());
	if(par_ass_id_v.empty()) continue;

	for(auto par_ass_id : par_ass_id_v) {
	  if (par_ass_id==kINVALID_SIZE) throw larbys();
	  _alg->Register(par_v.at(par_ass_id),plane);
	}
      } // end plane

      LAROCV_DEBUG() << "_alg->Match()" << std::endl;
      auto match_vv = _alg->Match();
      LAROCV_DEBUG() << "& recieved " << match_vv.size() << " matched particles" << std::endl;

      if (match_vv.empty()) continue;
      
      for( auto match_v : match_vv ) {
	for(auto& v : match_pcluster_v) v = nullptr;

	// Fill the match
	for (auto match : match_v) {
	  auto plane = match.first;
	  auto id    = match.second;
	  LAROCV_DEBUG() << "@plane=" << plane << " id=" << id << std::endl;
	  match_pcluster_v[plane] = _alg->Particle(plane,id);
	  LAROCV_DEBUG() << "...store ParticleCluster @" << match_pcluster_v[plane] << std::endl;
	}

	data::Particle particle;
	for(size_t plane=0; plane<3; ++plane) {
	  const auto& par = match_pcluster_v[plane];
	  if(!par) continue;
	  LAROCV_DEBUG() << " @ plane " << plane << " insert particle sz " << par->_ctor.size() << std::endl;
	  particle._par_v[plane] = *par;
	}

	// Associate
	LAROCV_DEBUG() << "associating" << std::endl;
	particle_data.emplace_back(std::move(particle));
	AssociateMany(vtx3d,particle_data.as_vector().back());

	for(size_t plane=0; plane<3; ++plane) {
	  const auto& par = match_pcluster_v[plane];
	  if(!par) continue;
	  AssociateMany(*par,particle_data.as_vector().back());
	} // end plane

      } // end this match
      LAROCV_DEBUG () << "next vertex" << std::endl;
    } // end this vertex    
    LAROCV_DEBUG() << "end" << std::endl;
  } // end process
  
}
#endif
