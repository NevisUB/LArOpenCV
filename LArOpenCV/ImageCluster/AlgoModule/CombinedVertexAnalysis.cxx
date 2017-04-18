#ifndef __COMBINEDVERTEXANALYSIS_CXX__
#define __COMBINEDVERTEXANALYSIS_CXX__

#include "CombinedVertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"

namespace larocv {
  
  /// Global larocv::CombinedVertexAnalysisFactory to register AlgoFactory
  static CombinedVertexAnalysisFactory __global_CombinedVertexAnalysisFactory__;

  void CombinedVertexAnalysis::Reset()
  {}
  
  void CombinedVertexAnalysis::_Configure_(const Config_t &pset) {

    //
    // Prepare algorithms via config
    //
    
    _track_vertex_algo_id = kINVALID_ALGO_ID;
    _shower_vertex_algo_id = kINVALID_ALGO_ID;
    
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
  
  void CombinedVertexAnalysis::_Process_()
  {
    LAROCV_DEBUG() << "start" << std::endl;

    //
    // 0) Prep
    //
    
    auto const img_v = ImageArray();

    // Get the assman
    auto& ass_man = AssManager();
    auto& vertex_data = AlgoData<data::Vertex3DArray>(0);
    const auto& shower_vertex_data       = AlgoData<data::Vertex3DArray>(_shower_vertex_algo_id,0);
    const auto& track_vertex_data        = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id,0);
    
    std::vector<const data::Vertex3D*> vertex3d_v;
    for(const auto& vtx : shower_vertex_data.as_vector()) vertex3d_v.push_back(&vtx);
    for(const auto& vtx : track_vertex_data.as_vector())  vertex3d_v.push_back(&vtx);
    
    for(const auto& vertex3d_ptr : vertex3d_v) {
      
      const auto& vtx3d = *vertex3d_ptr;
      vertex_data.push_back(vtx3d);
      auto& vtx3d_copy = vertex_data.as_vector().back();
      
      for(size_t plane=0;plane<_nplanes;++plane) {

	// This modules AlgoData
	auto& this_par_data = AlgoData<data::ParticleClusterArray>(plane+1);
	// auto& this_comp_data = AlgoData<data::TrackClusterCompoundArray>(_nplanes+plane+1);
	
	// Input algo data
	const auto& par_data   = AlgoData<data::ParticleClusterArray>(_particle_cluster_algo_id,plane);
	//const auto& track_comp_data  = AlgoData<data::TrackClusterCompoundArray>(_particle_cluster_algo_id,_nplanes+plane+1);

	// Get the associated track particles to this vertex, copy them to this algo data
	auto par_ass_id_v = ass_man.GetManyAss(vtx3d,par_data.ID());
	for(auto par_id : par_ass_id_v) {
	  auto track_par = par_data.as_vector().at(par_id);
	  // if (track_par.type!=data::ParticleType_t::kTrack) throw larbys("Not a track particle!");
	  // auto track_comp_id = ass_man.GetOneAss(track_par,track_comp_data.ID());
	  // const auto& track_comp    = track_comp_data.as_vector()[track_comp_id];
	  track_par.type=data::ParticleType_t::kShower;
	  this_par_data.push_back(track_par);
	  // comp_data.push_back(track_comp);
	  // Associate one particle to one track cluster compound
	  // AssociateOne(par_data.as_vector().back(),comp_data.as_vector().back());
	  // Associate this particle (one of many) to this vertex
	  AssociateMany(vtx3d_copy,this_par_data.as_vector().back());
	  // Associate this compound (one of many) to this vertex
	  // AssociateMany(vtx3d_copy,comp_data.as_vector().back());
	}

	/*
	// Get the associated shower particles to this vertex, copy them to this algo data
	auto shower_par_ass_id_v = ass_man.GetManyAss(vtx3d,shower_par_data.ID());	
	for(auto shower_par_id : shower_par_ass_id_v) {
	  const auto& par = shower_par_data.as_vector()[shower_par_id];
	  if (par.type==data::ParticleType_t::kShower) {
	    par_data.push_back(par);
	    AssociateMany(vtx3d_copy,par_data.as_vector().back());
	  }
	  else if (par.type==data::ParticleType_t::kTrack) {
	    par_data.push_back(par);
	    auto track_comp_id = ass_man.GetOneAss(par,shower_comp_data.ID());
	    if (track_comp_id==kINVALID_SIZE) throw larbys("Invalid comp id requested");
	    const auto& track_comp = shower_comp_data.as_vector()[track_comp_id];
	    AssociateMany(vtx3d_copy,par_data.as_vector().back());
	    comp_data.push_back(track_comp);
	    AssociateOne(par_data.as_vector().back(),comp_data.as_vector().back());
	  }
	}
	*/
      } // end this plane
    } // end this vertex
    
    LAROCV_DEBUG() << "Merged " << vertex3d_v.size() << " verticies" << std::endl;
    LAROCV_DEBUG() << "end" << std::endl;
  }

  bool CombinedVertexAnalysis::_PostProcess_() const
  { return true; }
   
}
#endif

