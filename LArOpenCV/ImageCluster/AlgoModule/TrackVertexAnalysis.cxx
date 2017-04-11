#ifndef __TRACKVERTEXANALYSIS_CXX__
#define __TRACKVERTEXANALYSIS_CXX__

#include "TrackVertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"

namespace larocv {

  /// Global larocv::TrackVertexAnalysisFactory to register AlgoFactory
  static TrackVertexAnalysisFactory __global_TrackVertexAnalysisFactory__;

  void TrackVertexAnalysis::Reset()
  {}
  
  void TrackVertexAnalysis::_Configure_(const Config_t &pset) {
    _track_vertex_algo_id = kINVALID_ALGO_ID;
    _track_particle_algo_id = kINVALID_ALGO_ID;
    
    auto track_vertex_algo_name = pset.get<std::string>("TrackVertexEstimate","");
    if (!track_vertex_algo_name.empty()) {
      _track_vertex_algo_id = this->ID(track_vertex_algo_name);
      if (_track_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given TrackVertexEstimate name is INVALID!");
    }
    
    auto track_particle_algo_name = pset.get<std::string>("VertexParticleClusterMaker","");
    if (!track_particle_algo_name.empty()) {
      _track_particle_algo_id = this->ID(track_particle_algo_name);
      if (_track_particle_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given VertexParticleClusterMaker name is INVALID!");
    }
    
    _min_time_wire_3d = pset.get<double>("MinTimeWireDistance3D",3.0); //cm

    _filter_by_xs  = pset.get<bool>("RequireXs",false);

    if (_filter_by_xs) {
      _required_xs_planes = pset.get<uint>("RequiredXsPlanes");
      _required_xs        = pset.get<uint>("RequiredXs");
    }
    
    Register(new data::Vertex3DArray);
    for(short pid=0;pid<3;++pid)
      Register(new data::ParticleClusterArray);
    for(short pid=0;pid<3;++pid)
      Register(new data::TrackClusterCompoundArray);
  }
  
  
  bool TrackVertexAnalysis::_PostProcess_(std::vector<cv::Mat>& img_v)
  {
    const auto& track_vtx_data = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id,0);
    
    std::vector<const data::Vertex3D*> wire_vtx_v;
    std::vector<const data::Vertex3D*> time_vtx_v;
    std::vector<const data::Vertex3D*> vtx_v;
    
    for(const auto& vtx3d : track_vtx_data.as_vector()) {
      if (vtx3d.type==data::VertexType_t::kTime) time_vtx_v.push_back(&vtx3d);
      else if (vtx3d.type==data::VertexType_t::kWire) wire_vtx_v.push_back(&vtx3d);
      else throw larbys("Vtx3D type is not wire or time... what?");
    }
    
    LAROCV_DEBUG() << "Identified... "
		   << "[wire] " << wire_vtx_v.size()
		   << " [time] " << time_vtx_v.size() << std::endl;

    // Merge nearbys wire vertex with time vertex
    _vertexana.MergeNearby(time_vtx_v,wire_vtx_v,_min_time_wire_3d);

    LAROCV_DEBUG() << "Merged wire w/ time vertex... "
		   << "[wire] " << wire_vtx_v.size()
		   << " [time] " << time_vtx_v.size() << std::endl;
    
    // Filter vertex to required number of crossing points
    if (_filter_by_xs) {
      _vertexana.FilterByCrossing(time_vtx_v,_required_xs_planes,_required_xs);
      _vertexana.FilterByCrossing(wire_vtx_v,_required_xs_planes,_required_xs);
    
      LAROCV_DEBUG() << "Required " << _required_xs << " on atleast " << _required_xs_planes << "... "
		     << "[wire] " << wire_vtx_v.size()
		     << " [time] " << time_vtx_v.size() << std::endl;
    }

    for (const auto vtx: wire_vtx_v)
      vtx_v.push_back(vtx);
    for (const auto vtx: time_vtx_v)
      vtx_v.push_back(vtx);

    auto& vtx_data = AlgoData<data::Vertex3DArray>(0);

    auto& assman = AssManager();

    for(auto& vtx3d_ : vtx_v) {
      LAROCV_DEBUG() << "On vtx3d: " << vtx3d_ << std::endl;
      
      //copy this vertex
      auto vtx3d_t = *vtx3d_;
      //place into this algo data
      vtx_data.emplace_back(std::move(vtx3d_t));
      //get reference to the last element
      auto& vtx3d = vtx_data.as_vector().back();
      
      for (size_t plane_id=0; plane_id<img_v.size(); plane_id++){
	LAROCV_DEBUG()<<"... plane"<<plane_id<<std::endl;
	  
	auto& particle_data = AlgoData<data::ParticleClusterArray>(1+plane_id);
	auto& compound_data = AlgoData<data::TrackClusterCompoundArray>(1+plane_id+3);

	const auto& particle_cluster_data = AlgoData<data::ParticleClusterArray>(_track_particle_algo_id,plane_id);
	const auto& track_cluster_data = AlgoData<data::TrackClusterCompoundArray>(_track_particle_algo_id,3+plane_id);
	  
	auto par_ass_idx_v = assman.GetManyAss(*vtx3d_, particle_cluster_data.ID());
	  
	LAROCV_DEBUG() << "... found " << par_ass_idx_v.size() << " associated track cluster compounds" << std::endl;
	for (auto particle_id : par_ass_idx_v){
	  const auto particle_ = particle_cluster_data.as_vector()[particle_id];
	  auto comp_id = assman.GetOneAss(particle_,track_cluster_data.ID());
	  auto compound = track_cluster_data.as_vector()[comp_id];
	  auto particle = particle_;
	  
	  compound_data.emplace_back(std::move(compound));
	  AssociateMany(vtx3d,compound_data.as_vector().back());
	  
	  particle_data.emplace_back(std::move(particle));
	  AssociateOne(particle_data.as_vector().back(),compound_data.as_vector().back());
	  
	  AssociateMany(vtx3d,particle_data.as_vector().back());
	  
	}//end this compound
      }//end this plane
    }//end this Vertex3D
    
    return true;
  }
   
}
#endif

