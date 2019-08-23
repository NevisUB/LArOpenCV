#ifndef __SHOWERVERTEXMERGER_CXX__
#define __SHOWERVERTEXMERGER_CXX__

#include "ShowerVertexMerger.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include <array>
#include <opencv2/imgproc.hpp>

namespace larocv {

  /// Global larocv::ShowerVertexMergerFactory to register AlgoFactory
  static ShowerVertexMergerFactory __global_ShowerVertexMergerFactory__;

  void ShowerVertexMerger::Reset()
  {}
  
  void ShowerVertexMerger::_Configure_(const Config_t &pset) {

    //
    // Prepare algorithms via config
    //
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
    for(size_t plane=0;plane<3;++plane) Register(new data::ParticleClusterArray);
    for(size_t plane=0;plane<3;++plane) Register(new data::TrackClusterCompoundArray);

  }

  bool ShowerVertexMerger::_PostProcess_() const
  { return true; }
  
  void ShowerVertexMerger::_Process_()
  {
    LAROCV_DEBUG() << "start" << std::endl;

    //
    // 0) Prep
    //
    
    auto img_v = ImageArray();
    
    // track and shower vertex algo data information
    const auto& track_vtx_data = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id,0);
    const auto& shower_vtx_data = AlgoData<data::Vertex3DArray>(_shower_vertex_algo_id,0);
    
    auto& ass_man = AssManager();
    
    auto& vtx_data = AlgoData<data::Vertex3DArray>(0);

    // Copy particle cluster array to simple contours
    std::vector<GEO2D_ContourArray_t> super_ctor_vv;
    super_ctor_vv.resize(3);
    for(size_t plane=0;plane<3;++plane) {
      auto& ctor_v=super_ctor_vv[plane];
      const auto& super_cluster_v = AlgoData<data::ParticleClusterArray>(_shower_super_cluster_maker_algo_id,plane).as_vector();      
      ctor_v.reserve(super_cluster_v.size());
      for(const auto& par : super_cluster_v) ctor_v.emplace_back(par._ctor);
    }
    
    for(const auto& shower_vtx : shower_vtx_data.as_vector()) {

      LAROCV_DEBUG() << "Observed vertex of type " << (uint) shower_vtx.type << std::endl;

      // Handle end of track vertex first
      if (shower_vtx.type==data::VertexType_t::kEndOfTrack) {
	
	// Get associated track vertex
	auto track_vertex_ass_id = ass_man.GetOneAss(shower_vtx,track_vtx_data.ID());
	if (track_vertex_ass_id==kINVALID_SIZE) throw larbys("Could not determine associated track-shower vertex");
	const auto& track_vtx = track_vtx_data.as_vector()[track_vertex_ass_id];
	
	std::array<bool,3> valid_plane_v{{true,true,true}};

	std::vector<std::vector<data::ParticleCluster> > pars_vv;
	std::vector<std::vector<data::TrackClusterCompound> > comp_vv;
	std::vector<std::vector<std::pair<size_t,size_t > > > par_comp_pvv;
	pars_vv.resize(3);
	comp_vv.resize(3);
	par_comp_pvv.resize(3);
	for(auto& v: pars_vv) v.reserve(5);
	for(auto& v: comp_vv) v.reserve(5);
	for(auto& v: par_comp_pvv) v.reserve(5);
	      
	for(size_t plane=0; plane < 3; ++plane) {

	  auto& pars_v = pars_vv[plane];
	  auto& comp_v = comp_vv[plane];
	  auto& par_comp_pv = par_comp_pvv[plane];
	  
	  const auto& super_ctor_v = super_ctor_vv[plane];
	  
	  // Get associated shower particles for the vertex on this plane (there may be 1 if > 1 crossing point)
	  const auto& shower_vtx_par_data = AlgoData<data::ParticleClusterArray>(_shower_vertex_shower_particle_algo_id,plane);
	  
	  auto shower_particle_ass_id_v = ass_man.GetManyAss(shower_vtx,shower_vtx_par_data.ID());
	  bool shower_exists=true;
	  if (shower_particle_ass_id_v.empty()) {
	    LAROCV_DEBUG() << "Could not find any showers for plane " << plane << std::endl;
	    shower_exists=false;
	  }

	  size_t super_index = kINVALID_SIZE;
	  GEO2D_Contour_t shower_ctor;

	  // If there are more than 1 associated shower, choose the one with the largest super contour
	  if (shower_particle_ass_id_v.size()!=1) {
	    // Get the largest super cluster
	    double max_area = -1 * kINVALID_DOUBLE;
	    for(auto shower_par_ass_id : shower_particle_ass_id_v) {
	      const auto& shower_par = shower_vtx_par_data.as_vector()[shower_par_ass_id];
	      auto const super_cluster_id = FindContainingContour(super_ctor_v, shower_par._ctor);
	      if (super_cluster_id == kINVALID_SIZE) continue;
	      double area = cv::contourArea(super_ctor_v[super_cluster_id]);
	      if (area>max_area) {
		max_area    = area;
		super_index = super_cluster_id;
	      }
	    }
	    if (super_index==kINVALID_SIZE) {
	      LAROCV_WARNING() << "Skipping this plane, there are >1 shower" << std::endl;
	      valid_plane_v[plane] = false;
	      continue;
	    }
	    shower_ctor = super_ctor_v[super_index];
	  }
	  else {
	    auto shower_par_ass_id = shower_particle_ass_id_v.front();
	    if (shower_par_ass_id == kINVALID_SIZE)
	      throw larbys("Could not find associated particle ID in particle algo data");	    
	    shower_ctor = shower_vtx_par_data.as_vector()[shower_par_ass_id]._ctor;
	  }
	  
	  // Find associated track particle associated with this vertex, there should be only one particle
	  const auto& track_vtx_par_data  = AlgoData<data::ParticleClusterArray>(_track_vertex_particle_algo_id,plane);
	  const auto& track_vtx_comp_data = AlgoData<data::TrackClusterCompoundArray>(_track_vertex_particle_algo_id,plane+3);
	  
	  auto track_particle_id = ass_man.GetOneAss(shower_vtx,track_vtx_par_data.ID());
	  if (track_particle_id == kINVALID_SIZE) {
	    LAROCV_WARNING() << "No single track particle associated with this shower vertex" << std::endl;
	    if (shower_exists)
	      throw larbys("Shower exists on this plane, but no track particle associated");
	  }

	  const auto& track_par = track_vtx_par_data.as_vector()[track_particle_id];

	  /*

	    Make decision to merge or not based on shower angle and track particle angle (near the vertex)
	    for now merge them no matter what, analysis code will go here
	  */
	  
	  // Join the two (track and shower) via merging
	  const auto& img = img_v[plane];
	  
	  auto merged_ctor = MergeByMask(shower_ctor,track_par._ctor,img,3);
	  LAROCV_DEBUG() << "Merge track ctor size " << track_par._ctor.size()
			 << " with shower ctor size " << shower_ctor.size()
			 << " into size " << merged_ctor.size() << " ctor" << std::endl;
	  
	  // Put in the shower cluster
	  data::ParticleCluster shower_cluster;
	  shower_cluster._ctor = std::move(merged_ctor);
	  shower_cluster.type  = data::ParticleType_t::kShower;
	  pars_v.emplace_back(std::move(shower_cluster));
	  
	  // Get the other associated track particles to this vertex
	  auto track_particle_id_v = ass_man.GetManyAss(track_vtx,track_vtx_par_data.ID());
	  if (track_particle_id_v.empty()) throw larbys("No particles associated?");
 
	  for(auto other_track_particle_id : track_particle_id_v) {

	    // Skip the one we just merged
	    if (other_track_particle_id == track_particle_id) continue;
	    
	    const auto& track_cluster = track_vtx_par_data.as_vector()[other_track_particle_id];
	    auto track_cluster_copy = track_cluster;
	    track_cluster_copy.type  = data::ParticleType_t::kTrack;
	    pars_v.emplace_back(std::move(track_cluster_copy));
	    
	    auto track_comp_id = ass_man.GetOneAss(track_cluster,track_vtx_comp_data.ID());
	    const auto& track_comp    = track_vtx_comp_data.as_vector()[track_comp_id];
	    comp_v.push_back(track_comp);

	    par_comp_pv.emplace_back(pars_v.size()-1,comp_v.size()-1);
	  }
	} //end this plane


	// Skip this guy if there are less than 2 valid planes
	short nvalid=0;
	for(auto v : valid_plane_v) nvalid+=(short)v;
	if (nvalid < 2) continue;

	// Make this track vertex a shower vertex
	auto shower_vertex = track_vtx;
	shower_vertex.type = data::VertexType_t::kEndOfTrack;
	vtx_data.emplace_back(std::move(shower_vertex));
	AssociateOne(vtx_data.as_vector().back(),track_vtx);
	
	for(size_t plane=0; plane < 3; ++plane) {
	  
	  auto& par_data  = AlgoData<data::ParticleClusterArray>(plane+1);
	  auto& par_v     = pars_vv[plane];
	  auto par_offset = par_data.as_vector().size();
	  
	  auto& comp_data = AlgoData<data::TrackClusterCompoundArray>(plane+3+1);
	  auto& comp_v    = comp_vv[plane];
	  auto comp_offset= comp_data.as_vector().size();
	  
	  for(auto& par : par_v)  {
	    par_data.emplace_back(std::move(par));
	    AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());	  
	  }

	  for(auto& comp : comp_v)  {
	    comp_data.emplace_back(std::move(comp));
	    AssociateMany(vtx_data.as_vector().back(),comp_data.as_vector().back());	  
	  }

	  auto& par_comp_pv = par_comp_pvv[plane];

	  for(auto par_comp_p : par_comp_pv) {
	    auto ass_par_id  = par_comp_p.first + par_offset;
	    auto ass_comp_id = par_comp_p.second + comp_offset;

	    assert(ass_par_id  < par_data.as_vector().size());
	    assert(ass_comp_id < comp_data.as_vector().size());
	    
	    AssociateOne(par_data.as_vector().at(ass_par_id),
			 comp_data.as_vector().at(ass_comp_id));
	  }

	} // end plane wise copy of particle and track compounds
      } // end this on edge vertex
      
      else  {

	// For this shower vertex connect tracks and showers coming from two difference VertexParticleClusterMaker modules
	vtx_data.push_back(shower_vtx);
	LAROCV_DEBUG() << "This vertex of type " << (uint)vtx_data.as_vector().back().type << std::endl;
		  
	for(size_t plane=0;plane<3;++plane) {
	  const auto& shower_vtx_track_par_data  = AlgoData<data::ParticleClusterArray>     (_shower_vertex_track_particle_algo_id ,plane);
	  const auto& shower_vtx_track_comp_data = AlgoData<data::TrackClusterCompoundArray>(_shower_vertex_track_particle_algo_id ,plane+3);
	  const auto& shower_vtx_shower_par_data = AlgoData<data::ParticleClusterArray>     (_shower_vertex_shower_particle_algo_id,plane);

	  auto& par_data  = AlgoData<data::ParticleClusterArray>(plane+1);
	  auto& comp_data = AlgoData<data::TrackClusterCompoundArray>(plane+1+3);
	  
	  auto track_particle_ass_id_v  = ass_man.GetManyAss(shower_vtx,shower_vtx_track_par_data.ID());
	  auto shower_particle_ass_id_v = ass_man.GetManyAss(shower_vtx,shower_vtx_shower_par_data.ID());
	  
	  LAROCV_DEBUG() << "Got " << shower_particle_ass_id_v.size() << " associated showers on plane " << plane << std::endl;	  

	  // Merge showers if they are parts of the same super cluster
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

	    // Merge them
	    for(size_t super_id=0;super_id<super_to_shower_vv.size();++super_id) {
	      if (super_to_shower_vv[super_id].empty()) continue;
	      data::ParticleCluster shower_cluster;
	      shower_cluster._ctor = super_ctor_v[super_id];
	      shower_cluster.type  = data::ParticleType_t::kShower;
	      par_data.emplace_back(std::move(shower_cluster));
	      AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());
	      LAROCV_DEBUG() << "Associated shower vertex id " << vtx_data.as_vector().back().ID()
			     << " with shower particle " << par_data.as_vector().back().ID() << std::endl;
	    }
	    
	  } else {
	    // Decided not to merge, get the showers one by one and copy them into my algo data
	    for(auto shower_par_ass_id : shower_particle_ass_id_v) {
	      auto shower_cluster = shower_vtx_shower_par_data.as_vector()[shower_par_ass_id];
	      shower_cluster.type = data::ParticleType_t::kShower;
	      par_data.emplace_back(std::move(shower_cluster));
	      AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());
	      LAROCV_DEBUG() << "Associated shower vertex id " << vtx_data.as_vector().back().ID()
			     << " with shower particle " << par_data.as_vector().back().ID() << std::endl;
	    }
	  }

	  // Get the clusters tracks, move them into my algo data
	  LAROCV_DEBUG() << "Got " << track_particle_ass_id_v.size() << " associated tracks on plane " << plane << std::endl;
	  for(auto track_par_ass_id : track_particle_ass_id_v) {
	    const auto& track_cluster = shower_vtx_track_par_data.as_vector()[track_par_ass_id];
	    auto track_cluster_copy   = track_cluster;

	    track_cluster_copy.type   = data::ParticleType_t::kTrack;
	    par_data.emplace_back(std::move(track_cluster_copy));
	    AssociateMany(vtx_data.as_vector().back(),par_data.as_vector().back());
	    
	    auto track_comp_id = ass_man.GetOneAss(track_cluster,shower_vtx_track_comp_data.ID());
	    if (track_comp_id==kINVALID_SIZE) throw larbys("Unassociated compound to track");
	    const auto& track_comp=shower_vtx_track_comp_data.as_vector()[track_comp_id];
	    comp_data.push_back(track_comp);
	    AssociateOne(par_data.as_vector().back(),comp_data.as_vector().back());
	    LAROCV_DEBUG() << "Associated shower vertex id " << vtx_data.as_vector().back().ID()
			   << " with track particle " << par_data.as_vector().back().ID() << std::endl;
	  } // end loop over tracks
	} // end this plane
      } // true shower vertex check, not end of track

    } // end loop over input shower like vertices
  }

}
#endif

