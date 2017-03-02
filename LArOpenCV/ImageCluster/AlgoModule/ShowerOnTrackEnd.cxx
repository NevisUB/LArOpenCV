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
				   const ::cv::Mat& img,
				   larocv::ImageMeta& meta,
				   larocv::ROI& roi)
  {
    _geo.ResetPlaneInfo(meta);
  }
  
  bool ShowerOnTrackEnd::_PostProcess_(const std::vector<const cv::Mat>& img_v)
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
      
      std::vector<std::array<size_t,2> > comp_shower_v;
      std::vector<bool> valid_plane_v(3,false);
      
      LAROCV_DEBUG() << "Vtx ID " << vtx3d_id << std::endl;
      for(size_t plane=0; plane<num_planes; ++plane) {
	auto compound_array = compound_v[plane];
	auto ass_idx_v = ass_man.GetManyAss(vtx3d,compound_array->ID());
	
	const auto& vtx2d = vtx3d.vtx2d_v[plane];
	LAROCV_DEBUG() << "[plane " << plane << "] 2D vertex @ " << vtx2d.pt << std::endl;
	for(auto const& compound_idx : ass_idx_v) {
	  auto const& compound = compound_array->as_vector().at(compound_idx);
	  LAROCV_DEBUG() << "this compound " << compound_idx << " has " << compound.get_atoms().size()
			 << " atoms w/ end point @ " << compound.end_pt() << std::endl;
	  // check if there is a shower here...
	  geo2d::Circle<float> circle;
	  circle.radius = _circle_default_radius;
	  circle.center = compound.end_pt();
	  auto const& img = img_v[plane];
	  auto xs_pt_v = QPointOnCircle(img,circle,10);
	  LAROCV_DEBUG() << "found  " << xs_pt_v.size()
			 << " crossing points on circle @ " << circle.center << " w/ rad " << circle.radius << std::endl;
	  if (xs_pt_v.empty()) continue;
	  comp_shower_v.push_back({{plane,compound_idx}});
	  valid_plane_v[plane] = true;
	}
      }
      size_t n_valid_planes = 0;
      for(auto valid_plane : valid_plane_v)
	n_valid_planes += (size_t)valid_plane;
      
      if (n_valid_planes < 2) {
	LAROCV_DEBUG() << "number of planes " << n_valid_planes << " invalid" << std::endl;
	continue;
      }
      
      LAROCV_DEBUG() << "Detected compounds across " << n_valid_planes << " planes via Xs w/ shower pixels" << std::endl;
      std::map<std::array<size_t,2>,double> score_m;
	       
      for(size_t comp1_idx=0;comp1_idx<comp_shower_v.size();++comp1_idx) {
	auto& comp1_p = comp_shower_v[comp1_idx];
	for(size_t comp2_idx=comp1_idx+1;comp2_idx<comp_shower_v.size();++comp2_idx) {
	  auto& comp2_p = comp_shower_v[comp2_idx];

	  auto comp1_pl = comp1_p[0];
	  auto comp2_pl = comp2_p[0];
	  auto comp1_id = comp1_p[1];
	  auto comp2_id = comp2_p[1];
	  
	  if (comp1_pl == comp2_pl) continue;

	  auto const& comp1 = compound_v[comp1_pl]->as_vector()[comp1_id];
	  auto const& comp2 = compound_v[comp2_pl]->as_vector()[comp2_id];

	  auto const& comp1_par_arr  = particle_v[comp1_pl];
	  auto const& comp2_par_arr  = particle_v[comp2_pl];
	  
	  auto comp1_par_idx = ass_man.GetOneAss(comp1,comp1_par_arr->ID());
	  auto comp2_par_idx = ass_man.GetOneAss(comp2,comp2_par_arr->ID());

	  const auto& comp1_par = comp1_par_arr->as_vector().at(comp1_par_idx);
	  const auto& comp2_par = comp2_par_arr->as_vector().at(comp2_par_idx);

	  auto comp1_pts_v = FindNonZero(MaskImage(Threshold(img_v[comp1_pl],-1,255),comp1_par._ctor,0,false));
	  auto comp2_pts_v = FindNonZero(MaskImage(Threshold(img_v[comp2_pl],-1,255),comp2_par._ctor,0,false)); 
	  
	  LAROCV_DEBUG() << "(" << comp1_id << "," << comp2_id << ") : "
			 << "(" << comp1_pts_v.size() << "," << comp2_pts_v.size() << ") : "
			 << "(" << comp1_pl << "," << comp2_pl << ") : "
			 << "(" << comp1_par._ctor.size() << "," << comp2_par._ctor.size() << ")"
			 << std::endl;

	  size_t overlap = 0;
	  std::vector<bool> used_2(comp2_pts_v.size(),false);
	  for(size_t cid1=0;cid1<comp1_pts_v.size();++cid1) {
	    for(size_t cid2=0;cid2<comp2_pts_v.size();++cid2) {
	      if (used_2[cid2]) continue;
	      bool cand = _geo.YZPoint(comp1_pts_v[cid1],comp1_pl,
				       comp2_pts_v[cid2],comp2_pl);
	      if (cand) {
		overlap++;
		used_2[cid2]=true;
		break;
	      }
	    }
	  }
	  LAROCV_DEBUG() << "Overlap is " << overlap << std::endl;
	  double overlap_d = overlap;
	  double denom_d = std::min(comp1_pts_v.size(),comp2_pts_v.size());
	  double frac = overlap_d / denom_d;
						
	  LAROCV_DEBUG() << "Coverage fraction is "<< overlap_d << " / " << denom_d << " = " << frac << std::endl;
	  score_m[{{comp1_idx,comp2_idx}}]=frac;
	} //end comp2
      } //end comp1



      //get the pair with the highest overlap fraction
      double highest_score = -1;
      std::array<size_t,2> highest_pair{{kINVALID_SIZE,kINVALID_SIZE}};
      for(const auto& score : score_m) {
	LAROCV_DEBUG() << "Score " << score.second
		       << " @ (" << score.first[0] << "," << score.first[1] << ")" << std::endl;
	if (score.second>highest_score) {
	  highest_pair  = score.first;
	  highest_score = score.second;
	}
      }

      std::vector<bool> used_planes(3,false);
      
      if ( highest_score > _overlap_fraction ) {
	LAROCV_DEBUG() << "Claiming a 3D vertex from particle end point comparison" << std::endl;
	data::Vertex3D vtx3d_f;
	vtx3d_f.type=data::VertexType_t::kEndOfTrack;
	vtx3d_f.num_planes=2;
	vtx3d_f.cvtx2d_v.resize(3);
	
	for(auto cid : highest_pair) {
	  auto& comp_p = comp_shower_v[cid];
	  auto plane_id = comp_p[0];
	  LAROCV_DEBUG() << "Using id " << cid << " @ plane " << plane_id << std::endl;
	  used_planes[plane_id]=true;
	  const auto& comp = compound_v[plane_id]->as_vector()[comp_p[1]];
	  auto& cvtx = vtx3d_f.cvtx2d_v[plane_id];
	  cvtx.center = comp.end_pt();
	  cvtx.radius = _circle_default_radius;
	  auto xs_v = QPointOnCircle(img_v[plane_id],geo2d::Circle<float>(cvtx.center,cvtx.radius),10);
	  auto& ppca_v = cvtx.xs_v;
	  for(auto & xs : xs_v) {
	    data::PointPCA ppca;
	    geo2d::Vector<float> pt_tmp(kINVALID_FLOAT,kINVALID_FLOAT);
	    geo2d::Line<float> line(pt_tmp,pt_tmp);
	    ppca = data::PointPCA(xs,line);
	    ppca_v.emplace_back(ppca);
	  }
	}
	
	// handle many plane case....
	size_t chosen_id = kINVALID_SIZE;
	if (n_valid_planes==3) {
	  double highest_score_0 = -1;
	  double highest_score_1 = -1;
	  std::array<size_t,2> highest_pair_0;
	  std::array<size_t,2> highest_pair_1;
	  
	  const auto high0 = highest_pair[0];
	  const auto high1 = highest_pair[1];
	  
	  //filter the score map for pairs which have plane IDs which are !BOTH! used
	  for(const auto& score : score_m) {
	    auto this0 = score.first[0];
	    auto this1 = score.first[1];
	    auto plane0 = comp_shower_v[this0][0];
	    auto plane1 = comp_shower_v[this1][0];
	    if (used_planes[plane0] and used_planes[plane1]) continue;
	    
	    //the first one is here, but second is not
	    if ((high0==this0 or high0==this1) and
		(high1!=this0 and high1!=this1)) {
	      if (score.second>highest_score_0) {
		highest_pair_0  = score.first;
		highest_score_0 = score.second;
	      }
	    }
	    
	    //the second one is here, but first is not
	    if ((high1==this0 or high1==this1) and
		(high0!=this0 and high0!=this1)) {
	      if (score.second>highest_score_1) {
		highest_pair_1  = score.first;
		highest_score_1 = score.second;
	      }
	    }
	    
	  }
	  
	  
	  LAROCV_DEBUG() << "Got highest scores for both chosen pair (" << high0 << "," << high1 << ")" << std::endl;
	  LAROCV_DEBUG() << high0 << " in pair (" << highest_pair_0[0] << "," << highest_pair_0[1] << ") w/ score " << highest_score_0 << std::endl;
	  LAROCV_DEBUG() << high1 << " in pair (" << highest_pair_1[0] << "," << highest_pair_1[1] << ") w/ score " << highest_score_1 << std::endl;
	  
	  auto chosen_pair = highest_score_0>highest_score_1 ? highest_pair_0 : highest_pair_1;
	  LAROCV_DEBUG() << "... chose pair (" << chosen_pair[0] << "," << chosen_pair[1] << ") --> adding additional plane" << std::endl;
	  if (chosen_pair[0] == high0 or chosen_pair[0] == high1) chosen_id = chosen_pair[1];
	  if (chosen_pair[1] == high0 or chosen_pair[1] == high1) chosen_id = chosen_pair[0];
	  if (chosen_id == kINVALID_SIZE)
	    throw larbys("Fucked");
	  
	  vtx3d_f.num_planes=3;
	  auto& comp_p = comp_shower_v[chosen_id];
	  auto plane_id = comp_p[0];
	  LAROCV_DEBUG() << "Using id " << chosen_id << " @ plane " << plane_id << std::endl;
	  used_planes[plane_id]=true;
	  const auto& comp = compound_v[plane_id]->as_vector()[comp_p[1]];
	  auto& cvtx = vtx3d_f.cvtx2d_v[plane_id];
	  cvtx.center = comp.end_pt();
	  cvtx.radius = _circle_default_radius;
	  auto xs_v = QPointOnCircle(img_v[plane_id],geo2d::Circle<float>(cvtx.center,cvtx.radius),10);
	  auto& ppca_v = cvtx.xs_v;
	  for(auto & xs : xs_v) {
	    data::PointPCA ppca;
	    geo2d::Vector<float> pt_tmp(kINVALID_FLOAT,kINVALID_FLOAT);
	    geo2d::Line<float> line(pt_tmp,pt_tmp);
	    ppca = data::PointPCA(xs,line);
	    ppca_v.emplace_back(ppca);
	  }
	  
	}
	LAROCV_DEBUG() << "Claimed..." << std::endl;
	vertex3d_v.emplace_back(std::move(vtx3d_f));
	AssociateOne(vtx3d,vertex3d_v.as_vector().back());
	
	//Associate this new vertex with particle
	for(auto cid : highest_pair) {
	  auto& comp_p = comp_shower_v[cid];
	  auto plane_id = comp_p[0];
	  auto comp_id  = comp_p[1];
	  
	  auto const& comp_par_arr  = particle_v[plane_id];
	  const auto& comp = compound_v[plane_id]->as_vector()[comp_id];
	  auto comp_par_idx = ass_man.GetOneAss(comp,comp_par_arr->ID());
	  const auto& comp_par = comp_par_arr->as_vector().at(comp_par_idx);
	  AssociateOne(vertex3d_v.as_vector().back(),comp_par);
	  LAROCV_DEBUG() << "Associated vertex " << vertex3d_v.as_vector().back().ID()
			 << " associated to... " << comp_par.ID() << std::endl;
	}
	//many plane case... associate with this particle
	if (n_valid_planes==3) {
	  auto& comp_p = comp_shower_v[chosen_id];
	  auto plane_id = comp_p[0];
	  auto comp_id  = comp_p[1];
	  
	  auto const& comp_par_arr  = particle_v[plane_id];
	  const auto& comp = compound_v[plane_id]->as_vector()[comp_id];
	  auto comp_par_idx = ass_man.GetOneAss(comp,comp_par_arr->ID());
	  const auto& comp_par = comp_par_arr->as_vector().at(comp_par_idx);
	  AssociateOne(vertex3d_v.as_vector().back(),comp_par);
	  LAROCV_DEBUG() << "Associated vertex " << vertex3d_v.as_vector().back().ID()
			 << " associated to... " << comp_par.ID() << std::endl;
	}
	LAROCV_WARNING() << "No association information set!" << std::endl;
      } // end overlap test
    } // end this vertex
    
    LAROCV_DEBUG() << "Inferred " << vertex3d_v.as_vector().size() << " vertices" << std::endl;
    LAROCV_DEBUG() << "end" << std::endl;
    return true;
  }
  
}
#endif

