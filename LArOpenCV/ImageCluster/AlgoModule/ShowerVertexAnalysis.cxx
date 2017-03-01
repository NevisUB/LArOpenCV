#ifndef __SHOWERVERTEXANALYSIS_CXX__
#define __SHOWERVERTEXANALYSIS_CXX__

#include "ShowerVertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {

  /// Global larocv::ShowerVertexAnalysisFactory to register AlgoFactory
  static ShowerVertexAnalysisFactory __global_ShowerVertexAnalysisFactory__;

  void ShowerVertexAnalysis::Reset()
  {}
  
  void ShowerVertexAnalysis::_Configure_(const Config_t &pset) {
    
    // _linear_track_algo_id = kINVALID_ALGO_ID;
    _track_vertex_algo_id = kINVALID_ALGO_ID;
    _track_vertex_particle_algo_id = kINVALID_ALGO_ID;
    _shower_vertex_algo_id = kINVALID_ALGO_ID;
    _shower_vertex_particle_algo_id = kINVALID_ALGO_ID;

    // auto linear_track_algo_name = pset.get<std::string>("LinearTrackAlgoName","");
    // if (!linear_track_algo_name.empty()) {
    //   _linear_track_algo_id = this->ID(linear_track_algo_name);
    //   if (_linear_track_algo_id==kINVALID_ALGO_ID)
    // 	throw larbys("Given LinearTrackAlgo name is INVALID!");
    // }
    
    auto track_vertex_algo_name = pset.get<std::string>("TrackVertexEstimate","");
    if (!track_vertex_algo_name.empty()) {
      _track_vertex_algo_id = this->ID(track_vertex_algo_name);
      if (_track_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given TrackVertexEstimate name is INVALID!");
    }

    auto track_vertex_particle_algo_name = pset.get<std::string>("TrackVertexParticleCluster","");
    if (!track_vertex_particle_algo_name.empty()) {
      _track_vertex_particle_algo_id = this->ID(track_vertex_particle_algo_name);
      if (_track_vertex_particle_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given TrackVertexParticleCluster name is INVALID!");
    }

    
    auto shower_vertex_algo_name = pset.get<std::string>("ShowerVertexEstimate","");
    if (!shower_vertex_algo_name.empty()) {
      _shower_vertex_algo_id = this->ID(shower_vertex_algo_name);
      if (_shower_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerVertexEstimate name is INVALID!");
    }
    
    auto shower_vertex_particle_algo_name = pset.get<std::string>("ShowerVertexParticleCluster","");
    if (!shower_vertex_particle_algo_name.empty()) {
      _shower_vertex_particle_algo_id = this->ID(shower_vertex_particle_algo_name);
      if (_shower_vertex_particle_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerVertexParticleCluster name is INVALID!");
    }
    
    Register(new data::Vertex3DArray);

    for(size_t planeid=0;planeid<3;++planeid)
      Register(new data::ParticleClusterArray);
    
  }
  
  
  bool ShowerVertexAnalysis::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    LAROCV_DEBUG() << "start" << std::endl;
    
    //linear track 
    //const auto& lin_track_data = AlgoData<data::LinearTrack3D>(_linear_track_algo_id,0);
    
    //track and shower vertex
    const auto& track_vtx_data = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id,0);
    const auto& shower_vtx_data = AlgoData<data::Vertex3DArray>(_shower_vertex_algo_id,0);
    
    auto& ass_man = AssManager();
    
    auto& vtx_data = AlgoData<data::Vertex3DArray>(0);
     
    for(const auto& shower_vtx : shower_vtx_data.as_vector()) {

      if (shower_vtx.type==data::VertexType_t::kEndOfTrack) {

	//get associated track vertex
	auto track_vertex_ass_id = ass_man.GetOneAss(shower_vtx,track_vtx_data.ID());
	if (track_vertex_ass_id==kINVALID_SIZE) throw larbys("Could not determine associated track-shower vertex");
	const auto& track_vtx = track_vtx_data.as_vector()[track_vertex_ass_id];

	auto shower_vertex = track_vtx;
	shower_vertex.type = data::VertexType_t::kShower;
	vtx_data.emplace_back(std::move(shower_vertex));
	
	for(size_t plane=0;plane<3;++plane) {
	  LAROCV_DEBUG() << "On plane " << plane << std::endl;
	  auto& par_data = AlgoData<data::ParticleClusterArray>(plane+1);
	  
	  //get associated shower for this vertex on this plane
	  const auto& shower_vtx_par_data = AlgoData<data::ParticleClusterArray>(_shower_vertex_particle_algo_id,plane);

	  auto shower_particle_ass_id_v = ass_man.GetManyAss(shower_vtx,shower_vtx_par_data.ID());
	  bool shower_exists=true;
	  if (shower_particle_ass_id_v.empty()) {
	    LAROCV_DEBUG() << "Could not find any showers for plane " << plane << std::endl;
	    shower_exists=false;
	  }

	  if (shower_particle_ass_id_v.size()!=1) {
	    LAROCV_WARNING() << "Skipping this plane, there are >1 shower" << std::endl;
	    continue;
	  }

	  auto shower_par_ass_id = shower_particle_ass_id_v.front();
	  if (shower_par_ass_id == kINVALID_SIZE)
	    throw larbys("Could not find associated particle ID in particle algo data");
	  
	  const auto& shower_par = shower_vtx_par_data.as_vector()[shower_par_ass_id];

	  //get associated track particle associated with this vertex, there may be only one
	  const auto& track_vtx_par_data = AlgoData<data::ParticleClusterArray>(_track_vertex_particle_algo_id,plane);
	  	  
	  auto track_particle_id = ass_man.GetOneAss(shower_vtx,track_vtx_par_data.ID());
	  if (track_particle_id == kINVALID_SIZE) {
	    LAROCV_WARNING() << "No single track particle associated with this shower vertex" << std::endl;
	    if (shower_exists)
	      throw larbys("Shower exists on this plane, but no track particle associated");
	  }
	  
	  const auto& track_par = track_vtx_par_data.as_vector()[track_particle_id];

	  /*

	    Make decision to merge or not based on trunk angle
	    for now just do it

	   */

	  
	  //Join the two via merge
	  const auto& img = img_v[plane];
	  auto merged_ctor = MergeByMask(shower_par._ctor,track_par._ctor,img,3);
	  LAROCV_DEBUG() << "Merge track ctor size " << track_par._ctor.size()
			 << " with shower ctor size " << shower_par._ctor.size()
			 << " into size " << merged_ctor.size() << " ctor" << std::endl;
	  
	  //put in the shower cluster
	  data::ParticleCluster shower_cluster;
	  shower_cluster._ctor=std::move(merged_ctor);
	  shower_cluster.type=data::ParticleType_t::kShower;
	  par_data.emplace_back(std::move(shower_cluster));
	  AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());
	  LAROCV_DEBUG() << "Associated track=>shower vertex id " << vtx_data.as_vector().back().ID()
			 << " with shower particle " << par_data.as_vector().back().ID() << std::endl;
	  
	  //get the other track particle(s)
	  auto track_particle_id_v = ass_man.GetManyAss(track_vtx,track_vtx_par_data.ID());
	  if (track_particle_id_v.empty()) throw larbys("No particles associated?");

	  for(auto other_track_particle_id : track_particle_id_v) {
	    // skip the one we just merged...
	    if (other_track_particle_id == track_particle_id) {
	      LAROCV_DEBUG() << "Skipping " << track_particle_id << " since we used it already" << std::endl;
	      continue;
	    }
	    LAROCV_DEBUG() << "... other track particle index " << other_track_particle_id << std::endl;
	    auto track_cluster = track_vtx_par_data.as_vector()[other_track_particle_id];
	    track_cluster.type=data::ParticleType_t::kTrack;
	    par_data.emplace_back(std::move(track_cluster));
	    AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());
	    LAROCV_DEBUG() << "Associated track=>shower vertex id " << vtx_data.as_vector().back().ID()
			   << " with track particle " << par_data.as_vector().back().ID() << std::endl;
	  }
	} //end this plane
      } // end this on edge vertex
      
      else  {
	// shower vertex
	vtx_data.push_back(shower_vtx);
	for(size_t plane=0;plane<3;++plane) {
	  const auto& shower_vtx_par_data = AlgoData<data::ParticleClusterArray>(_shower_vertex_particle_algo_id,plane);
	  auto& par_data = AlgoData<data::ParticleClusterArray>(plane+1);

	  auto shower_particle_ass_id_v = ass_man.GetManyAss(shower_vtx,shower_vtx_par_data.ID());
	  
	  for(auto shower_par_ass_id : shower_particle_ass_id_v) {
	    auto shower_cluster = shower_vtx_par_data.as_vector()[shower_par_ass_id];
	    shower_cluster.type=data::ParticleType_t::kShower;
	    par_data.emplace_back(std::move(shower_cluster));
	    AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());
	    LAROCV_DEBUG() << "Associated shower=>shower vertex id " << vtx_data.as_vector().back().ID()
			   << " with shower " << par_data.as_vector().back().ID() << std::endl;
	  }

	  //how to get the linear track
	  
	  
	} // end this plane
      }
    }
    

    LAROCV_DEBUG() << "end" << std::endl;
    return true;
  }
   
}
#endif

