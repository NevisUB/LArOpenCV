#ifndef __SHOWERVERTEXMERGER_CXX__
#define __SHOWERVERTEXMERGER_CXX__

#include "ShowerVertexMerger.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {

  /// Global larocv::ShowerVertexMergerFactory to register AlgoFactory
  static ShowerVertexMergerFactory __global_ShowerVertexMergerFactory__;

  void ShowerVertexMerger::Reset()
  {}
  
  void ShowerVertexMerger::_Configure_(const Config_t &pset) {
    
    _track_vertex_algo_id = kINVALID_ALGO_ID;
    _track_vertex_particle_algo_id = kINVALID_ALGO_ID;
    _shower_vertex_algo_id = kINVALID_ALGO_ID;
    _shower_vertex_track_particle_algo_id = kINVALID_ALGO_ID;
    _shower_vertex_shower_particle_algo_id = kINVALID_ALGO_ID;
    _shower_super_cluster_maker_algo_id = kINVALID_ALGO_ID;
    
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
    
    auto shower_vertex_particle_1_algo_name = pset.get<std::string>("ShowerVertexTrackParticleCluster","");
    if (!shower_vertex_particle_1_algo_name.empty()) {
      _shower_vertex_track_particle_algo_id = this->ID(shower_vertex_particle_1_algo_name);
      if (_shower_vertex_track_particle_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerVertexParticleCluster1 name is INVALID!");
    }

    auto shower_vertex_shower_particle_algo_name = pset.get<std::string>("ShowerVertexShowerParticleCluster","");
    if (!shower_vertex_shower_particle_algo_name.empty()) {
      _shower_vertex_shower_particle_algo_id = this->ID(shower_vertex_shower_particle_algo_name);
      if (_shower_vertex_shower_particle_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerVertexParticleCluster2 name is INVALID!");
    }
    
    auto shower_super_cluster_maker_algo_name = pset.get<std::string>("ShowerSuperClusterAlgo","");
    if (!shower_super_cluster_maker_algo_name.empty()) {
      _shower_super_cluster_maker_algo_id = this->ID(shower_super_cluster_maker_algo_name);
      if (_shower_super_cluster_maker_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerSuperClusterAlgo name is INVALID!");
    }


    _merge_compatible_showers=pset.get<bool>("MergeCompatibleShowers");
    
    Register(new data::Vertex3DArray);
    for(size_t planeid=0;planeid<3;++planeid)
      Register(new data::ParticleClusterArray);
    for(size_t planeid=0;planeid<3;++planeid)
      Register(new data::TrackClusterCompoundArray);

  }
  
  
  bool ShowerVertexMerger::_PostProcess_(std::vector<cv::Mat>& img_v)
  {
    LAROCV_DEBUG() << "start" << std::endl;
    
    //track and shower vertex
    const auto& track_vtx_data = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id,0);
    const auto& shower_vtx_data = AlgoData<data::Vertex3DArray>(_shower_vertex_algo_id,0);
    
    auto& ass_man = AssManager();
    
    auto& vtx_data = AlgoData<data::Vertex3DArray>(0);

    std::vector<GEO2D_ContourArray_t> super_ctor_vv;
    super_ctor_vv.resize(3);
    for(size_t plane=0;plane<3;++plane) {
      const auto& super_cluster_v = AlgoData<data::ParticleClusterArray>(_shower_super_cluster_maker_algo_id,plane).as_vector();      
      auto& ctor_v=super_ctor_vv[plane];
      ctor_v.reserve(super_cluster_v.size());
      for(const auto& super_cluster : super_cluster_v)
	ctor_v.emplace_back(super_cluster._ctor);
    }
    
    for(const auto& shower_vtx : shower_vtx_data.as_vector()) {

      LAROCV_DEBUG() << "Observed vertex of type " << (uint) shower_vtx.type << std::endl;

      // End of track vertex first
      if (shower_vtx.type==data::VertexType_t::kEndOfTrack) {
	
	//get associated track vertex
	auto track_vertex_ass_id = ass_man.GetOneAss(shower_vtx,track_vtx_data.ID());
	if (track_vertex_ass_id==kINVALID_SIZE) throw larbys("Could not determine associated track-shower vertex");
	const auto& track_vtx = track_vtx_data.as_vector()[track_vertex_ass_id];

	auto shower_vertex = track_vtx;
	shower_vertex.type = data::VertexType_t::kEndOfTrack;
	vtx_data.emplace_back(std::move(shower_vertex));
	AssociateOne(vtx_data.as_vector().back(),track_vtx);
	
	LAROCV_DEBUG() << "This vertex of type " << (uint)vtx_data.as_vector().back().type << std::endl;
	
	for(size_t plane=0;plane<3;++plane) {
	  LAROCV_DEBUG() << "On plane " << plane << std::endl;
	  auto& par_data = AlgoData<data::ParticleClusterArray>(plane+1);
	  auto& comp_data = AlgoData<data::TrackClusterCompoundArray>(plane+1+3);
	  const auto& super_ctor_v = super_ctor_vv[plane];
	  
	  //get associated shower for this vertex on this plane
	  const auto& shower_vtx_par_data = AlgoData<data::ParticleClusterArray>(_shower_vertex_shower_particle_algo_id,plane);
	  
	  auto shower_particle_ass_id_v = ass_man.GetManyAss(shower_vtx,shower_vtx_par_data.ID());
	  bool shower_exists=true;
	  if (shower_particle_ass_id_v.empty()) {
	    LAROCV_DEBUG() << "Could not find any showers for plane " << plane << std::endl;
	    shower_exists=false;
	  }

	  size_t super_index = kINVALID_SIZE;
	  GEO2D_Contour_t shower_ctor;
	  if (shower_particle_ass_id_v.size()!=1) {
	    //get the largest super cluster
	    double max_area = 0;
	    for(auto shower_par_ass_id : shower_particle_ass_id_v) {
	      const auto& shower_par = shower_vtx_par_data.as_vector()[shower_par_ass_id];
	      auto const super_cluster_id = FindContainingContour(super_ctor_v, shower_par._ctor);
	      double area = cv::contourArea(super_ctor_v[super_cluster_id]);
	      LAROCV_DEBUG() << "Shower par " << shower_par_ass_id
			     << " ass with super "<< super_cluster_id
			     << " with area " << area
			     << " with size " << super_ctor_v[super_cluster_id].size() << std::endl;
	      if (area>max_area) {
		max_area = area;
		super_index=super_cluster_id;
		LAROCV_DEBUG() << "Max area " << max_area << " @ super index " << super_index << std::endl;
	      }
	    }
	    if (super_index==kINVALID_SIZE) {
	      LAROCV_WARNING() << "Skipping this plane, there are >1 shower" << std::endl;
	      continue;
	    }
	    LAROCV_DEBUG() << "Found max area ("<<max_area<<") @ index " << super_index << std::endl;
	    shower_ctor = super_ctor_v[super_index];
	  }
	  else {
	    auto shower_par_ass_id = shower_particle_ass_id_v.front();
	    if (shower_par_ass_id == kINVALID_SIZE)
	      throw larbys("Could not find associated particle ID in particle algo data");	    
	    shower_ctor = shower_vtx_par_data.as_vector()[shower_par_ass_id]._ctor;
	  }

	  
	  //get associated track particle associated with this vertex, there may be only one
	  const auto& track_vtx_par_data = AlgoData<data::ParticleClusterArray>(_track_vertex_particle_algo_id,plane);
	  const auto& track_vtx_comp_data = AlgoData<data::TrackClusterCompoundArray>(_track_vertex_particle_algo_id,plane+3);
	  
	  auto track_particle_id = ass_man.GetOneAss(shower_vtx,track_vtx_par_data.ID());
	  if (track_particle_id == kINVALID_SIZE) {
	    LAROCV_WARNING() << "No single track particle associated with this shower vertex" << std::endl;
	    if (shower_exists)
	      throw larbys("Shower exists on this plane, but no track particle associated");
	  }
	  
	  const auto& track_par = track_vtx_par_data.as_vector()[track_particle_id];

	  /*

	    Make decision to merge or not based on trunk angle
	    for now just do it, maybe we can pick this up in analysis stage

	   */
	  
	  //Join the two via merge
	  const auto& img = img_v[plane];
	  
	  auto merged_ctor = MergeByMask(shower_ctor,track_par._ctor,img,3);
	  LAROCV_DEBUG() << "Merge track ctor size " << track_par._ctor.size()
			 << " with shower ctor size " << shower_ctor.size()
			 << " into size " << merged_ctor.size() << " ctor" << std::endl;
	  
	  //put in the shower cluster
	  data::ParticleCluster shower_cluster;
	  shower_cluster._ctor=std::move(merged_ctor);
	  shower_cluster.type=data::ParticleType_t::kShower;
	  par_data.emplace_back(std::move(shower_cluster));
	  AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());
	  LAROCV_DEBUG() << "Associated shower=>shower vertex id " << vtx_data.as_vector().back().ID()
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
	    const auto& track_cluster = track_vtx_par_data.as_vector()[other_track_particle_id];
	    auto track_cluster_copy=track_cluster;
	    track_cluster_copy.type=data::ParticleType_t::kTrack;
	    par_data.emplace_back(std::move(track_cluster_copy));
	    AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());

	    auto track_comp_id = ass_man.GetOneAss(track_cluster,track_vtx_comp_data.ID());
	    auto track_comp = track_vtx_comp_data.as_vector()[track_comp_id];
	    comp_data.emplace_back(std::move(track_comp));
	    AssociateOne(par_data.as_vector().back(),comp_data.as_vector().back());
			 
	    LAROCV_DEBUG() << "Associated track=>shower vertex id " << vtx_data.as_vector().back().ID()
			   << " with track particle " << par_data.as_vector().back().ID() << std::endl;
	  }
	} //end this plane
      } // end this on edge vertex
      
      else  {
	// shower vertex, connect tracks and showers,
	// if shower is not unique lets replace it with the corresponding
	// shower super cluster

	vtx_data.push_back(shower_vtx);
	LAROCV_DEBUG() << "This vertex of type " << (uint)vtx_data.as_vector().back().type << std::endl;
		  
	for(size_t plane=0;plane<3;++plane) {
	  const auto& shower_vtx_track_par_data = AlgoData<data::ParticleClusterArray>(_shower_vertex_track_particle_algo_id,plane);
	  const auto& shower_vtx_track_comp_data = AlgoData<data::TrackClusterCompoundArray>(_shower_vertex_track_particle_algo_id,plane+3);
	  const auto& shower_vtx_shower_par_data = AlgoData<data::ParticleClusterArray>(_shower_vertex_shower_particle_algo_id,plane);

	  auto& par_data = AlgoData<data::ParticleClusterArray>(plane+1);
	  auto& comp_data = AlgoData<data::TrackClusterCompoundArray>(plane+1+3);
	  
	  auto track_particle_ass_id_v = ass_man.GetManyAss(shower_vtx,shower_vtx_track_par_data.ID());
	  auto shower_particle_ass_id_v = ass_man.GetManyAss(shower_vtx,shower_vtx_shower_par_data.ID());
	  
	  LAROCV_DEBUG() << "Got " << shower_particle_ass_id_v.size() << " associated showers on plane " << plane << std::endl;	  

	  if (_merge_compatible_showers) {
	    const auto& super_ctor_v = super_ctor_vv[plane];
	    std::vector<std::vector<size_t> > super_to_shower_vv;
	    super_to_shower_vv.resize(super_ctor_v.size());
	    for(auto shower_par_ass_id : shower_particle_ass_id_v) {
	      const auto& shower_par = shower_vtx_shower_par_data.as_vector()[shower_par_ass_id];
	      auto const super_cluster_id = FindContainingContour(super_ctor_v, shower_par._ctor);
	      if (super_cluster_id==kINVALID_SIZE) 
		{ LAROCV_WARNING()<<"No super cluster found? skip"<<std::endl; continue; }
	      super_to_shower_vv[super_cluster_id].push_back(shower_par_ass_id);
	    }
	    //merge
	    for(size_t super_id=0;super_id<super_to_shower_vv.size();++super_id) {
	      if (super_to_shower_vv[super_id].empty()) continue;
	      data::ParticleCluster shower_cluster;
	      shower_cluster._ctor = super_ctor_v[super_id];
	      shower_cluster.type=data::ParticleType_t::kShower;
	      par_data.emplace_back(std::move(shower_cluster));
	      AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());
	      LAROCV_DEBUG() << "Associated shower vertex id " << vtx_data.as_vector().back().ID()
			     << " with shower particle " << par_data.as_vector().back().ID() << std::endl;
	    }
	  } else {
	    for(auto shower_par_ass_id : shower_particle_ass_id_v) {
	      auto shower_cluster = shower_vtx_shower_par_data.as_vector()[shower_par_ass_id];
	      shower_cluster.type=data::ParticleType_t::kShower;
	      par_data.emplace_back(std::move(shower_cluster));
	      AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());
	      LAROCV_DEBUG() << "Associated shower vertex id " << vtx_data.as_vector().back().ID()
			     << " with shower particle " << par_data.as_vector().back().ID() << std::endl;
	    }
	  }
	  LAROCV_DEBUG() << "Got " << track_particle_ass_id_v.size() << " associated tracks on plane " << plane << std::endl;
	  for(auto track_par_ass_id : track_particle_ass_id_v) {
	    const auto& track_cluster = shower_vtx_track_par_data.as_vector()[track_par_ass_id];
	    auto track_cluster_copy=track_cluster;
	    track_cluster_copy.type=data::ParticleType_t::kTrack;
	    par_data.emplace_back(std::move(track_cluster_copy));
	    AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());
	    auto track_comp_id = ass_man.GetOneAss(track_cluster,shower_vtx_track_comp_data.ID());
	    const auto& track_comp=shower_vtx_track_comp_data.as_vector()[track_comp_id];
	    comp_data.push_back(track_comp);
	    AssociateOne(par_data.as_vector().back(),comp_data.as_vector().back());
	    LAROCV_DEBUG() << "Associated shower vertex id " << vtx_data.as_vector().back().ID()
			   << " with track particle " << par_data.as_vector().back().ID() << std::endl;
	  }
	} // end this plane
      }
    }
    

    LAROCV_DEBUG() << "end" << std::endl;
    return true;
  }
   
}
#endif

