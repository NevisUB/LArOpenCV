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

    _overlap_fraction = pset.get<float>("OverlapFraction",0.7);

    _circle_default_radius= pset.get<uint>("CircleDefaultRadius",10);
    _pca_size = pset.get<uint>("PCABoxSize",5);
    
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

  void ShowerOnTrackEnd::_Process_(const larocv::Cluster2DArray_t& clusters,
				   ::cv::Mat& img,
				   larocv::ImageMeta& meta,
				   larocv::ROI& roi)
  {
    _VertexAnalysis.ResetPlaneInfo(meta);
  }
  
  bool ShowerOnTrackEnd::_PostProcess_(std::vector<cv::Mat>& img_v)
  {
    
    LAROCV_DEBUG() << "start" << std::endl;
    auto num_planes = img_v.size();
    auto& ass_man = AssManager();
    
    // my 3d vertex
    auto& vertex3d_v  = AlgoData<data::Vertex3DArray>(0);
    
    // 3D vertex
    auto const& input_vtx3d_v = AlgoData<data::Vertex3DArray>(_vertex3d_id,0);
    auto const& vtx3d_v = input_vtx3d_v.as_vector();

    // compounds per plane
    std::vector<const data::ParticleClusterArray*> particle_v;
    std::vector<const data::TrackClusterCompoundArray*> compound_v;
    for(size_t plane=0; plane<num_planes; ++plane)  {
      particle_v.push_back(&(AlgoData<data::ParticleClusterArray>(_compound_id,plane)));
      compound_v.push_back(&(AlgoData<data::TrackClusterCompoundArray>(_compound_id,plane+num_planes)));
    }
    
    // get many association
    LAROCV_DEBUG() << "Inspect " << vtx3d_v.size() << std::endl;
    for(size_t vtx3d_id=0; vtx3d_id<vtx3d_v.size(); ++vtx3d_id) {
      const auto& vtx3d = vtx3d_v[vtx3d_id];

      LAROCV_DEBUG() << "This vertex type " << (uint)vtx3d.type << std::endl;
      
      std::array<bool,3> valid_plane_v;
      for(auto& valid_plane : valid_plane_v) valid_plane=false;

      std::vector<std::vector<const data::ParticleCluster*> > pars_ptr_vv;
      pars_ptr_vv.resize(3);
      for(auto& pars_ptr_v : pars_ptr_vv) pars_ptr_v.clear();
      
      for(size_t plane=0; plane<num_planes; ++plane) {
	auto particle_array = particle_v[plane];
	auto compound_array = compound_v[plane];
	auto part_ass_idx_v = ass_man.GetManyAss(vtx3d,particle_array->ID());
	auto comp_ass_idx_v = ass_man.GetManyAss(vtx3d,compound_array->ID());
	
	for(auto const& compound_idx : comp_ass_idx_v) {
	  auto const& compound = compound_array->as_vector().at(compound_idx);

	  // check if there is a shower here...
	  geo2d::Circle<float> circle;
	  circle.radius = _circle_default_radius;
	  circle.center = compound.end_pt();
	  auto const& img = img_v[plane];
	  auto xs_pt_v = QPointOnCircle(img,circle,10);

	  if (xs_pt_v.empty()) continue;

	  auto par_id = ass_man.GetOneAss(compound,particle_array->ID());
	  pars_ptr_vv[plane].push_back(&(particle_array->as_vector().at(par_id)));
	  valid_plane_v[plane] = true;
	}
      }
	size_t n_valid_planes = 0;
      for(auto valid_plane : valid_plane_v)
	n_valid_planes += (size_t)valid_plane;
      
      if (n_valid_planes < 2) continue;

      std::vector<cv::Mat> track_img_v;
      track_img_v.resize(img_v.size());

      for(size_t plane=0; plane<img_v.size(); ++plane) {
	auto& track_img = track_img_v[plane];
	track_img =  BlankImage(img_v[plane],0);
	for(const auto& par_ptr : pars_ptr_vv.at(plane)) {
	  track_img += MaskImage(BlankImage(img_v[plane],255),par_ptr->_ctor,0,false);
	}
      }

      auto match_vv = _VertexAnalysis.MatchClusters(pars_ptr_vv,track_img_v,_overlap_fraction,1,1,false);

      LAROCV_DEBUG() << "Found " << match_vv.size() << " matches" << std::endl;
      if (match_vv.size()!=1) {
	LAROCV_DEBUG() << "More that one match across " << n_valid_planes << " found for particle touching shower?" << std::endl;
	continue;
      }

      const auto& match_v = match_vv.front();
      LAROCV_DEBUG() << "Examining " << match_v.size() << " particle match" << std::endl;
	
      LAROCV_DEBUG() << "Claiming a 3D vertex from particle end point comparison" << std::endl;
      data::Vertex3D vtx3d_f;
      vtx3d_f.type=data::VertexType_t::kEndOfTrack;
      vtx3d_f.num_planes=match_v.size();
      vtx3d_f.cvtx2d_v.resize(3);
      
      for(auto match : match_v) {
	auto plane_id = match.first;
	auto par_id   = match.second;
	LAROCV_DEBUG() << "Using id " << par_id << " @ plane " << plane_id << std::endl;
	
	const auto& pars_ptr_v = pars_ptr_vv[plane_id];
	const auto& par = *(pars_ptr_v[par_id]);
	auto compound_array = compound_v[plane_id];
	auto comp_id = ass_man.GetOneAss(par,compound_array->ID());
	const auto& comp = compound_array->as_vector().at(comp_id);
	
	auto& cvtx = vtx3d_f.cvtx2d_v[plane_id];
	cvtx.center = comp.end_pt();
	cvtx.radius = _circle_default_radius;
	auto circle = geo2d::Circle<float>(cvtx.center,cvtx.radius);
	auto xs_v = QPointOnCircle(img_v[plane_id],circle,10);
	auto& ppca_v = cvtx.xs_v;
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

      for(auto match : match_v) {
	auto plane_id = match.first;
	auto par_id   = match.second;

	const auto& pars_ptr_v = pars_ptr_vv[plane_id];
	const auto& par = *(pars_ptr_v[par_id]);

	auto compound_array = compound_v[plane_id];
	auto comp_id = ass_man.GetOneAss(par,compound_array->ID());
	const auto& comp = compound_array->as_vector().at(comp_id);

	LAROCV_DEBUG() << "Associating vertex with par id " << par_id << std::endl;
	AssociateOne(vertex3d_v.as_vector().back(),par);
	LAROCV_DEBUG() << "Associating vertex with comp id " << comp_id << std::endl;
	AssociateOne(vertex3d_v.as_vector().back(),comp);
      }

    } // end this vertex
    
    LAROCV_DEBUG() << "Inferred " << vertex3d_v.as_vector().size() << " vertices" << std::endl;
    LAROCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
}
#endif

