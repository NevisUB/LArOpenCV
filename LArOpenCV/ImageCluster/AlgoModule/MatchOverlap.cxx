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
    
    _vertex_algo_offset = kINVALID_SIZE;
    _particle_cluster_algo_offset = kINVALID_SIZE;
    
    _vertex_algo_offset = pset.get<size_t>("VertexAlgoOffset");
    _particle_cluster_algo_offset = pset.get<size_t>("ParticleClusterAlgoOffset");

    _match_coverage            = pset.get<float>("MatchCoverage",0.5);
    _match_particles_per_plane = pset.get<float>("MatchParticlesPerPlane",2);
    _match_min_number          = pset.get<float>("MatchMinimumNumber",2);
    _match_check_type          = pset.get<bool>("MatchCheckType",true);
    _match_weight_by_size      = pset.get<bool>("MatchWeightBySize",false);

    Register(new data::ParticleArray);
  }
  
  bool MatchOverlap::_PostProcess_() const
  { return true; }

  void MatchOverlap::_Process_() {

    auto wire_img_v  = ImageArray(ImageSetID_t::kImageSetWire);
    auto meta_v      = MetaArray(ImageSetID_t::kImageSetWire);
    
    for(size_t plane=0; plane<3; ++plane) 
      _VertexAnalysis.ResetPlaneInfo(meta_v.at(plane));    

    auto& assman = AssManager();
    
    auto& particle_data = AlgoData<data::ParticleArray>(0);

    const auto& vtx3d_arr = AlgoData<data::Vertex3DArray>(_vertex_algo_id,_vertex_algo_offset);
    const auto& vtx3d_v = vtx3d_arr.as_vector();

    LAROCV_DEBUG() << "Matching " << vtx3d_v.size() << " vertices" << std::endl;
    
    for(const auto& vtx3d : vtx3d_v) {
      data::VertexTrackInfoCollection vtx_trk_info;
      vtx_trk_info.vtx3d = &vtx3d;
      LAROCV_DEBUG() << "Vertex: " << &vtx3d << std::endl;

      vtx_trk_info.particle_vv.resize(3);

      for(size_t plane=0; plane<3; ++plane) {
	auto& particle_v = vtx_trk_info.particle_vv[plane];
	
	const auto& par_data = AlgoData<data::ParticleClusterArray>(_particle_cluster_algo_id,
								    _particle_cluster_algo_offset+plane);
	const auto& par_v = par_data.as_vector();

	auto par_ass_id_v = assman.GetManyAss(vtx3d,par_data.ID());
	if(par_ass_id_v.empty()) continue;

	for(auto par_ass_id : par_ass_id_v) {
	  if (par_ass_id==kINVALID_SIZE) throw larbys();
	  particle_v.push_back(&par_v.at(par_ass_id));
	}
      }

      auto match_vv = _VertexAnalysis.MatchClusters(vtx_trk_info.particle_vv,
						    wire_img_v,
						    _match_coverage,              // required coverage
						    _match_particles_per_plane,   // requires # particles per plane
						    _match_min_number,            // min number of matches
						    _match_check_type,            // ensure particle type is same
						    _match_weight_by_size);       // weight match by particle n pixels)

      LAROCV_DEBUG() << "... found " << match_vv.size() << " matched particles" << std::endl;
      if (match_vv.empty()) continue;
      
      for( auto match_v : match_vv ) {
	
	std::array<size_t,3> plane_arr, id_arr;
	plane_arr = id_arr = {{kINVALID_SIZE, kINVALID_SIZE, kINVALID_SIZE}};
	
	std::array<const larocv::data::ParticleCluster*, 3> pcluster_v {{nullptr,nullptr,nullptr}};
	
	// Fill the match
	for (auto match : match_v) {
	  
	  auto plane = match.first;
	  auto id    = match.second;
	  plane_arr[plane] = plane;
	  id_arr[plane]    = id;
	  
	  pcluster_v[plane] = vtx_trk_info.particle_vv[plane][id];
	}

	data::Particle particle;
	for(size_t plane=0; plane<3; ++plane) {
	  const auto& par = pcluster_v[plane];
	  if(!par) continue;
	  LAROCV_DEBUG() << " @ plane " << plane << " insert particle sz " << par->_ctor.size() << std::endl;
	  particle._par_v[plane] = *par;
	}

	// Associate
	particle_data.emplace_back(std::move(particle));
	AssociateMany(vtx3d,particle_data.as_vector().back());

	for(size_t plane=0; plane<3; ++plane) {
	  const auto& par = pcluster_v[plane];
	  if(!par) continue;
	  AssociateMany(*par,particle_data.as_vector().back());
	}	
      }
      
    } // end this vertex    
  } // end process
  
}
#endif
