#ifndef __SHOWERONTRACKEND_CXX__
#define __SHOWERONTRACKEND_CXX__

#include "ShowerOnTrackEnd.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {

  /// Global larocv::ShowerOnTrackEndFactory to register AlgoFactory
  static ShowerOnTrackEndFactory __global_ShowerOnTrackEndFactory__;

  void ShowerOnTrackEnd::Reset()
  {}
  
  void ShowerOnTrackEnd::_Configure_(const Config_t &pset) {

    _overlap_fraction = 0.7;

    _circle_default_radius=10;
    _pca_size = 5;
    
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

      std::vector<std::pair<size_t,size_t> > comp_shower_v;
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
	  comp_shower_v.emplace_back(plane,compound_idx);
	  valid_plane_v[plane] = true;
	}
      }
      size_t n_valid_planes = 0;
      for(auto valid_plane : valid_plane_v)
	n_valid_planes += (size_t)valid_plane;
      
      if (n_valid_planes != 2) {
	LAROCV_DEBUG() << "number of planes " << n_valid_planes << " invalid" << std::endl;
	continue;
      }
      
      LAROCV_DEBUG() << "Detected compounds across " << n_valid_planes << " planes via Xs w/ shower pixels" << std::endl;
      std::map<double,std::pair<size_t,size_t> > score_m;
	       
      for(size_t comp1_idx=0;comp1_idx<comp_shower_v.size();++comp1_idx) {
	auto& comp1_p = comp_shower_v[comp1_idx];
	for(size_t comp2_idx=comp1_idx+1;comp2_idx<comp_shower_v.size();++comp2_idx) {
	  auto& comp2_p = comp_shower_v[comp2_idx];

	  auto comp1_pl = comp1_p.first;
	  auto comp2_pl = comp2_p.first;
	  auto comp1_id = comp1_p.second;
	  auto comp2_id = comp2_p.second;
	  
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
	  double frac = (double) overlap / (double) std::min(comp1_pts_v.size(),
							     comp2_pts_v.size());
	  LAROCV_DEBUG() << "Coverage fraction is " << frac << std::endl;
	  score_m[frac]=std::make_pair(comp1_idx,comp2_idx);
	} //end comp2
      } //end comp1

      auto max_score = score_m.rbegin();
      if ( max_score->first > _overlap_fraction ) {
	LAROCV_DEBUG() << "Claiming a 3D vertex from particle end point comparison" << std::endl;
	data::Vertex3D vtx3d;
	vtx3d.type=3;
	vtx3d.num_planes=2;

	auto comp1_id = max_score->second.first;
	auto comp2_id = max_score->second.second;
	
	auto& comp1_p = comp_shower_v[comp1_id];
	auto& comp2_p = comp_shower_v[comp2_id];

	auto plane1 = comp1_p.first;
	auto plane2 = comp2_p.first;
	
	const auto& comp1 = compound_v[plane1]->as_vector()[comp1_p.second];
	const auto& comp2 = compound_v[plane2]->as_vector()[comp2_p.second];
	
	vtx3d.cvtx2d_v.resize(3);

	auto& cvtx1 = vtx3d.cvtx2d_v[plane1];
	auto& cvtx2 = vtx3d.cvtx2d_v[plane2];
	
	cvtx1.center = comp1.end_pt();
	cvtx1.radius = _circle_default_radius;
	
	cvtx2.center = comp2.end_pt();
	cvtx2.radius = _circle_default_radius;
	  
	auto xs1_v = QPointOnCircle(img_v[plane1],geo2d::Circle<float>(cvtx1.center,cvtx1.radius),10);
	auto xs2_v = QPointOnCircle(img_v[plane2],geo2d::Circle<float>(cvtx2.center,cvtx2.radius),10);
	
	auto& ppca1 = cvtx1.xs_v;
	auto& ppca2 = cvtx2.xs_v;

	for(auto & xs1 : xs1_v) 
	  ppca1.emplace_back(data::PointPCA(xs1,SquarePCA(img_v[plane1],xs1,
							  _pca_size,_pca_size)));

	for(auto & xs2 : xs2_v) 
	  ppca2.emplace_back(data::PointPCA(xs2,SquarePCA(img_v[plane2],xs2,
							  _pca_size,_pca_size)));
	
	LAROCV_DEBUG() << "Claimed..." << std::endl;
	vertex3d_v.emplace_back(std::move(vtx3d));
	LAROCV_WARNING() << "No association information set!" << std::endl;
      } // end overlap test
    } // end this vertex
    
    LAROCV_DEBUG() << "Inferred " << vertex3d_v.as_vector().size() << " vertices" << std::endl;
    LAROCV_DEBUG() << "end" << std::endl;
    return true;
  }
   
}
#endif

