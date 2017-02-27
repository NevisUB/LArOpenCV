#ifndef __TRACKVERTEXANALYSIS_CXX__
#define __TRACKVERTEXANALYSIS_CXX__

#include "TrackVertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

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
    
    //number of planes to have... 
    _required_xs_planes = pset.get<uint>("RequiredXsPlanes",2);
    //... this many crossing points
    _required_xs = pset.get<uint>("RequiredXs",2);
    
    Register(new data::Vertex3DArray);
  }
  
  
  bool TrackVertexAnalysis::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    const auto& track_vtx_data = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id,0);
    
    std::vector<const data::Vertex3D*> wire_vtx_v;
    std::vector<const data::Vertex3D*> time_vtx_v;
    std::vector<const data::Vertex3D*> vtx_v;

    for(const auto& vtx3d : track_vtx_data.as_vector()) {
      if (vtx3d.type==0) time_vtx_v.push_back(&vtx3d);
      else if (vtx3d.type==1) wire_vtx_v.push_back(&vtx3d);
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
    _vertexana.FilterByCrossing(time_vtx_v,_required_xs_planes,_required_xs);
    _vertexana.FilterByCrossing(wire_vtx_v,_required_xs_planes,_required_xs);
    
    LAROCV_DEBUG() << "Required " << _required_xs << " on atleast " << _required_xs_planes << "... "
		   << "[wire] " << wire_vtx_v.size()
		   << " [time] " << time_vtx_v.size() << std::endl;
    

    for (const auto vtx: wire_vtx_v)
      vtx_v.push_back(vtx);
    for (const auto vtx: time_vtx_v)
      vtx_v.push_back(vtx);


    auto & assman = AssManager();

    for (const auto vtx3d:vtx_v){
  
      for (uint p_id =0;p_id <3;p_id++){

	auto& track_cluster_data = AlgoData<data::TrackClusterCompoundArray>(_track_vertex_algo_id,3+p_id);
	auto ass_idx_v = assman.GetManyAss(*vtx3d, track_cluster_data.ID());
	for (auto compound_id : ass_idx_v){
	  auto& compound = track_cluster_data.as_vector()[compound_id];
	  for (auto atomic : compound){
	    auto pca = CalcPCA(atomic);
	    auto dqdx = _atomicanalysis.AtomdQdX(img_v[p_id], atomic, pca, atomic.start_pt(), atomic.end_pt());
	    atomic.set_dqdx(dqdx);
	    LAROCV_DEBUG()<<'size of dqdx is '<<dqdx.size()<<std::endl;
	  }
	}
      }
    }

    auto& vtx_data = AlgoData<data::Vertex3DArray>(0);

    for(const auto vtx : vtx_v)
      vtx_data.push_back(*vtx);
    
    
    return true;
  }
   
}
#endif

