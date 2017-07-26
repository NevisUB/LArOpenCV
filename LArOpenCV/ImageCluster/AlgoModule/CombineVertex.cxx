#ifndef __COMBINEVERTEX_CXX__
#define __COMBINEVERTEX_CXX__

#include "CombineVertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"

namespace larocv {
  
  /// Global larocv::CombineVertexFactory to register AlgoFactory
  static CombineVertexFactory __global_CombineVertexFactory__;

  void CombineVertex::Reset()
  {}
  
  void CombineVertex::_Configure_(const Config_t &pset) {

    //
    // Prepare algorithms via config
    //
    
    _track_vertex_algo_id = kINVALID_ALGO_ID;
    _shower_vertex_algo_id = kINVALID_ALGO_ID;
    _particle_cluster_algo_id = kINVALID_ALGO_ID;
    
    auto track_vertex_algo_name = pset.get<std::string>("TrackVertexEstimateAlgo","");
    if (!track_vertex_algo_name.empty()) {
      _track_vertex_algo_id = this->ID(track_vertex_algo_name);
      if (_track_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given TrackVertex name is INVALID!");
    }

    auto shower_vertex_algo_name = pset.get<std::string>("ShowerVertexEstimateAlgo","");
    if (!shower_vertex_algo_name.empty()) {
      _shower_vertex_algo_id = this->ID(shower_vertex_algo_name);
      if (_shower_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerVertex name is INVALID!");
    }

    auto particle_cluster_algo_name = pset.get<std::string>("ParticleClusterAlgo","");
    if (!particle_cluster_algo_name.empty()) {
      _particle_cluster_algo_id = this->ID(particle_cluster_algo_name);
      if (_particle_cluster_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ParticleCluster name is INVALID!");
    }
    
    _nplanes=3;
    Register(new data::Vertex3DArray);
    for(size_t plane=0;plane<_nplanes;++plane) Register(new data::ParticleClusterArray);
    for(size_t plane=0;plane<_nplanes;++plane) Register(new data::TrackClusterCompoundArray);
  }
  
  void CombineVertex::_Process_()
  {
    LAROCV_DEBUG() << "start" << std::endl;

    //
    // 0) Prep
    //
    
    auto const img_v = ImageArray();

    // Get the assman
    auto& ass_man = AssManager();
    auto& vertex_data = AlgoData<data::Vertex3DArray>(0);
    
    std::vector<const data::Vertex3D*> vertex_v;
    
    if (_shower_vertex_algo_id!=kINVALID_ALGO_ID) {
      const auto& shower_vertex_data = AlgoData<data::Vertex3DArray>(_shower_vertex_algo_id,0);
      for(const auto& vtx : shower_vertex_data.as_vector()) vertex_v.push_back(&vtx);
    }

    if (_track_vertex_algo_id!=kINVALID_ALGO_ID) {
      const auto& track_vertex_data  = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id,0);
      for(const auto& vtx : track_vertex_data.as_vector()) vertex_v.push_back(&vtx);
    }

    for(size_t vertex_id=0; vertex_id < vertex_v.size(); ++vertex_id) {

      const auto vertex3d_ptr = vertex_v[vertex_id];

      // Make a copy into module algo data
      const auto& vtx3d = *vertex3d_ptr;
      vertex_data.push_back(vtx3d);
      auto& vtx3d_copy = vertex_data.as_vector().back();

      for(size_t plane=0; plane<_nplanes; ++plane) {
	
	// This module AlgoData
	auto& this_par_data = AlgoData<data::ParticleClusterArray>(plane+1);

	// Input algo data
	const auto& par_data   = AlgoData<data::ParticleClusterArray>(_particle_cluster_algo_id,plane);

	// Get the associated track particles to this vertex, copy them to this algo data
	auto par_ass_id_v = ass_man.GetManyAss(vtx3d,par_data.ID());
	for(auto par_id : par_ass_id_v) {
	  auto par = par_data.as_vector().at(par_id);

	  // All particles are unknown type at the moment until we look at the relative pixel fractions
	  par.type = data::ParticleType_t::kUnknown;
	  
	  LAROCV_DEBUG() << "Inserting particle @ plane " << plane << " sz " << par._ctor.size() << std::endl;
	  if(par._ctor.empty()) {
	    LAROCV_DEBUG() << "... not inserting" << std::endl;
	    continue;
	  }
	  this_par_data.push_back(par);
	  AssociateMany(vtx3d_copy,this_par_data.as_vector().back());
	}

      } // end this plane
    } // end this vertex
    
    LAROCV_DEBUG() << "Merged " << vertex_v.size() << " verticies" << std::endl;
    LAROCV_DEBUG() << "end" << std::endl;
  }
  
  bool CombineVertex::_PostProcess_() const
  { return true; }
   
}
#endif

