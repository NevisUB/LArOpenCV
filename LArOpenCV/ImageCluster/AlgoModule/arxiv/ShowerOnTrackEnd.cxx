#ifndef __SHOWERONTRACKEND_CXX__
#define __SHOWERONTRACKEND_CXX__

#include "ShowerOnTrackEnd.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include <array>

namespace larocv {

  /// Global larocv::ShowerOnTrackEndFactory to register AlgoFactory
  static ShowerOnTrackEndFactory __global_ShowerOnTrackEndFactory__;

  void ShowerOnTrackEnd::Reset()
  {}
  
  void ShowerOnTrackEnd::_Configure_(const Config_t &pset) {

    //
    // Prepare algorithms via config
    //

    // Required amount of overlap between particles to considered for match
    _overlap_fraction = pset.get<float>("OverlapFraction",0.7);

    // Default radius for determing the crossing point with the shower image
    _circle_default_radius= pset.get<uint>("CircleDefaultRadius",10);
    
    _vertex3d_id = kINVALID_ALGO_ID;
    _compound_id = kINVALID_ALGO_ID;
    
    auto name_track_vertex   = pset.get<std::string>("Vertex3DName","");
    auto name_track_compound = pset.get<std::string>("TrackParticleName","");

    if (!name_track_vertex.empty()) {
      _vertex3d_id      = this->ID(name_track_vertex);
      if (_vertex3d_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Vertex3D algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }
    if (!name_track_compound.empty()) {
      _compound_id      = this->ID(name_track_compound);
      if (_compound_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "TrackParticle algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    Register(new data::Vertex3DArray);
  }

  bool ShowerOnTrackEnd::_PostProcess_() const
  { return true; }

  void ShowerOnTrackEnd::_Process_()
  {
    //
    // 0) Prep
    //

    auto trk_img_v = ImageArray(ImageSetID_t::kImageSetTrack);
    auto shr_img_v = ImageArray(ImageSetID_t::kImageSetShower);
    
    // Set up the vertex analysis module for this image set
    for(auto const& meta : MetaArray())
      _VertexAnalysis.ResetPlaneInfo(meta);
      
    auto num_planes = shr_img_v.size();
    auto& ass_man = AssManager();
    
    // This modules 3d vertex algorithm data
    auto& vertex3d_v  = AlgoData<data::Vertex3DArray>(0);
    
    // Input modules 3d vertex algorithm data
    auto const& vtx3d_v = AlgoData<data::Vertex3DArray>(_vertex3d_id,0).as_vector();
    
    // TrackClusterCompounds per plane
    std::vector<const data::ParticleClusterArray*> particle_v;
    std::vector<const data::TrackClusterCompoundArray*> compound_v;
    for(size_t plane=0; plane<num_planes; ++plane)  {
      particle_v.push_back(&(AlgoData<data::ParticleClusterArray>(_compound_id,plane)));
      compound_v.push_back(&(AlgoData<data::TrackClusterCompoundArray>(_compound_id,plane+num_planes)));
    }

    //
    // 1) Search for shower pixels on the end of track cluster compounds
    //
    
    LAROCV_DEBUG() << "Inspect " << vtx3d_v.size() << " vertices" << std::endl;
    for(size_t vtx3d_id=0; vtx3d_id<vtx3d_v.size(); ++vtx3d_id) {

      // Look at this vertex
      const auto& vtx3d = vtx3d_v[vtx3d_id];
      
      std::array<bool,3> valid_plane_v;
      for(auto& valid_plane : valid_plane_v) valid_plane=false;

      std::vector<std::vector<const data::ParticleCluster*> > pars_ptr_vv;
      pars_ptr_vv.resize(3);
      for(auto& pars_ptr_v : pars_ptr_vv) pars_ptr_v.clear();

      // For each of the planes, look at the tracks
      for(size_t plane=0; plane<num_planes; ++plane) {
	auto particle_array = particle_v[plane];
	auto compound_array = compound_v[plane];
	auto part_ass_idx_v = ass_man.GetManyAss(vtx3d,particle_array->ID());
	auto comp_ass_idx_v = ass_man.GetManyAss(vtx3d,compound_array->ID());

	// For the tracks associated to this vertex on this plane
	for(auto const& compound_idx : comp_ass_idx_v) {
	  auto const& compound = compound_array->as_vector().at(compound_idx);

	  // Check if there is a shower here at this particle end point using a circle
	  geo2d::Circle<float> circle;
	  circle.radius = _circle_default_radius;
	  circle.center = compound.end_pt();
	  auto const& img = shr_img_v[plane];
	  auto xs_pt_v = QPointOnCircle(img,circle,10);

	  // Is there a crossing point?
	  if (xs_pt_v.empty()) continue;
	  
	  // Yes, get this particle and store it's pointer
	  auto par_id = ass_man.GetOneAss(compound,particle_array->ID());
	  pars_ptr_vv[plane].push_back(&(particle_array->as_vector().at(par_id)));
	  valid_plane_v[plane] = true;
	}
      }

      // Was there at least 1 particle found on atleast two planes?
      size_t n_valid_planes = 0;
      for(auto valid_plane : valid_plane_v)
	n_valid_planes += (size_t)valid_plane;
      
      if (n_valid_planes < 2) continue;

      //
      // 1) Given the particles per plane for this vertex, attempt matching
      //
      
      // Require _overlap_fraction threshold & atleast 1 particle on that plane if there is a match
      auto match_vv = _VertexAnalysis.MatchClusters(pars_ptr_vv,trk_img_v,_overlap_fraction,1,1,false);
      
      LAROCV_DEBUG() << "Found " << match_vv.size() << " matches" << std::endl;
      if (match_vv.size()!=1) {
	LAROCV_DEBUG() << "More that one match across " << n_valid_planes
		       << " found for particle touching shower?" << std::endl;
	continue;
      }

      const auto& match_v = match_vv.front();
      LAROCV_DEBUG() << "Examining " << match_v.size() << " particle match" << std::endl;
      
      LAROCV_DEBUG() << "Claiming a 3D vertex from particle end point comparison" << std::endl;
      data::Vertex3D vtx3d_f;
      vtx3d_f.type       = data::VertexType_t::kEndOfTrack;
      vtx3d_f.num_planes = match_v.size();
      vtx3d_f.cvtx2d_v.resize(3);
      
      // For each particle per plane
      for(auto match : match_v) {
	auto plane_id = match.first;
	auto par_id   = match.second;
	LAROCV_DEBUG() << "Using id " << par_id << " @ plane " << plane_id << std::endl;

	// Get this particle
	const auto& pars_ptr_v = pars_ptr_vv[plane_id];
	const auto& par = *(pars_ptr_v[par_id]);

	// Get the associated compound
	auto compound_array = compound_v[plane_id];
	auto comp_id = ass_man.GetOneAss(par,compound_array->ID());
	const auto& comp = compound_array->as_vector().at(comp_id);

	// Get the circle vertex on this plane, and compute
	// the crossing point with the shower image
	auto& cvtx = vtx3d_f.cvtx2d_v[plane_id];
	cvtx.center = comp.end_pt();
	cvtx.radius = _circle_default_radius;
	auto circle = geo2d::Circle<float>(cvtx.center,cvtx.radius);
	auto xs_v = QPointOnCircle(shr_img_v[plane_id],circle,10);
	auto& ppca_v = cvtx.xs_v;
	// For each crossing point, make a PointPCA, required for ParticleClusterMaker
	for(auto & xs : xs_v) {
	  data::PointPCA ppca;
	  geo2d::Vector<float> pt_tmp(kINVALID_FLOAT,kINVALID_FLOAT);
	  geo2d::Line<float> line(pt_tmp,pt_tmp);
	  ppca = data::PointPCA(xs,line);
	  ppca_v.emplace_back(ppca);
	}
      }
      
      vertex3d_v.emplace_back(std::move(vtx3d_f));
      AssociateOne(vertex3d_v.as_vector().back(),vtx3d);

      // Now that the vertex is pushed into algodata, lets set
      // the associations
      for(auto match : match_v) {
	auto plane_id = match.first;
	auto par_id   = match.second;

	const auto& pars_ptr_v = pars_ptr_vv[plane_id];
	const auto& par = *(pars_ptr_v[par_id]);
	
	auto compound_array = compound_v[plane_id];
	auto comp_id = ass_man.GetOneAss(par,compound_array->ID());
	const auto& comp = compound_array->as_vector().at(comp_id);
	
	AssociateOne(vertex3d_v.as_vector().back(),par);
	AssociateOne(vertex3d_v.as_vector().back(),comp);
      }
    } // end this vertex
    
    
  }
}
#endif

