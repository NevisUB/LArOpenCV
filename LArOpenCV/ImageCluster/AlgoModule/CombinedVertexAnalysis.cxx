#ifndef __COMBINEDVERTEXANALYSIS_CXX__
#define __COMBINEDVERTEXANALYSIS_CXX__

#include "CombinedVertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

namespace larocv {

  /// Global larocv::CombinedVertexAnalysisFactory to register AlgoFactory
  static CombinedVertexAnalysisFactory __global_CombinedVertexAnalysisFactory__;

  void CombinedVertexAnalysis::Reset()
  {}
  
  void CombinedVertexAnalysis::_Configure_(const Config_t &pset) {

    _track_vertex_algo_id = kINVALID_ALGO_ID;
    _shower_vertex_algo_id = kINVALID_ALGO_ID;

    auto track_vertex_algo_name = pset.get<std::string>("TrackVertex","");
    if (!track_vertex_algo_name.empty()) {
      _track_vertex_algo_id = this->ID(track_vertex_algo_name);
      if (_track_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given TrackVertex name is INVALID!");
    }

    auto shower_vertex_algo_name = pset.get<std::string>("ShowerVertex","");
    if (!shower_vertex_algo_name.empty()) {
      _shower_vertex_algo_id = this->ID(shower_vertex_algo_name);
      if (_shower_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerVertex name is INVALID!");
    }

    _nplanes=3;
    Register(new data::Vertex3DArray);
    for(size_t planeid=0;planeid<_nplanes;++planeid)
      Register(new data::ParticleClusterArray);
    for(size_t planeid=0;planeid<_nplanes;++planeid)
      Register(new data::TrackClusterCompoundArray);
    
  }
  
  bool CombinedVertexAnalysis::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    LAROCV_DEBUG() << "start" << std::endl;
    auto& ass_man = AssManager();
    auto& vertex_data = AlgoData<data::Vertex3DArray>(0);
    const auto& shower_vertex_data = AlgoData<data::Vertex3DArray>(_shower_vertex_algo_id,0);
    const auto& track_vertex_data = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id,0);

    std::vector<const data::Vertex3D*> vertex3d_ptr_v;
    for(const auto& vtx3d : shower_vertex_data.as_vector())
      vertex3d_ptr_v.push_back(&vtx3d);
    
    for(const auto& vtx3d : track_vertex_data.as_vector())
      vertex3d_ptr_v.push_back(&vtx3d);

    for(const auto& vertex3d_ptr : vertex3d_ptr_v) {
      const auto& vtx3d = *vertex3d_ptr;
      vertex_data.push_back(vtx3d);
      auto& vtx3d_copy = vertex_data.as_vector().back();
      
      for(size_t plane=0;plane<_nplanes;++plane) {
	auto& par_data = AlgoData<data::ParticleClusterArray>(plane+1);
	const auto& track_par_data = AlgoData<data::ParticleClusterArray>(_track_vertex_algo_id,plane+1);
	const auto& shower_par_data = AlgoData<data::ParticleClusterArray>(_shower_vertex_algo_id,plane+1);
	
	auto& comp_data = AlgoData<data::TrackClusterCompoundArray>(_nplanes+plane+1);
	const auto& track_comp_data = AlgoData<data::TrackClusterCompoundArray>(_track_vertex_algo_id,_nplanes+plane+1);
	const auto& shower_comp_data = AlgoData<data::TrackClusterCompoundArray>(_shower_vertex_algo_id,_nplanes+plane+1);

	auto track_par_ass_id_v = ass_man.GetManyAss(vtx3d,track_par_data.ID());
	for(auto track_par_id : track_par_ass_id_v) {
	  const auto& track_par = track_par_data.as_vector()[track_par_id];
	  if (track_par.type!=data::ParticleType_t::kTrack) throw larbys("Not a track particle!");
	  auto track_comp_id = ass_man.GetOneAss(track_par,track_comp_data.ID());
	  const auto& track_comp = track_comp_data.as_vector()[track_comp_id];
	  par_data.push_back(track_par);
	  comp_data.push_back(track_comp);
	  AssociateOne(par_data.as_vector().back(),comp_data.as_vector().back());
	  AssociateMany(vtx3d_copy,par_data.as_vector().back());
	  AssociateMany(vtx3d_copy,comp_data.as_vector().back());
	}
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
      }
    }

    LAROCV_DEBUG() << "Merged " << vertex3d_ptr_v.size() << " verticies" << std::endl;
    
    LAROCV_DEBUG() << "end" << std::endl;
    return true;
  }
   
}
#endif

