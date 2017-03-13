#ifndef __VERTEXANALYSIS_CXX__
#define __VERTEXANALYSIS_CXX__

#include "VertexAnalysis.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/Base/BaseUtil.h"
#include "LArOpenCV/ImageCluster/Base/MatchBookKeeper.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

using larocv::larbys;

namespace larocv {

  VertexAnalysis::VertexAnalysis() : _geo()
  {}

  void
  VertexAnalysis::Configure(const Config_t &pset)
  {}


  void
  VertexAnalysis::MergeNearby(const std::vector<const data::Vertex3D*>& vtx1_v,
			      std::vector<const data::Vertex3D*>& vtx2_v,
			      double dist3d) {

    std::set<size_t> rm_vtx2_s;
    for(const auto time_vtx : vtx1_v) {
      for(size_t vtx2_id=0;vtx2_id<vtx2_v.size();++vtx2_id) {
	auto vtx2_vtx = vtx2_v[vtx2_id];

	if ( data::Distance(*time_vtx,*vtx2_vtx) < dist3d)
	  rm_vtx2_s.insert(vtx2_id);
      }
    }
    std::vector<const data::Vertex3D*> vtx2_temp_v;
    for(size_t vtx2_id=0;vtx2_id<vtx2_v.size();++vtx2_id) {
      if (rm_vtx2_s.find(vtx2_id) != rm_vtx2_s.end()) continue;
      vtx2_temp_v.push_back(vtx2_v[vtx2_id]);
    }
    std::swap(vtx2_v,vtx2_temp_v);
  }

  void
  VertexAnalysis::MergeNearby(std::vector<const data::Vertex3D*>& vtx1_v,
			      double dist3d) {
    throw larbys("Not implemented");
  }


  std::vector<std::vector<std::pair<size_t,size_t> > >
  VertexAnalysis::MatchClusters(const std::vector<std::vector<data::ParticleCluster> >& pars_vv,
				const std::vector<const cv::Mat>& img_v,
				float threshold,
				size_t required_per_plane,
				size_t required_matches) {

    // For now I have to make a pointer
    std::vector<std::vector<const data::ParticleCluster*> > pars_ptr_vv;
    pars_ptr_vv.resize(pars_vv.size());

    for(size_t planeid=0;planeid<pars_vv.size();++planeid) {
      auto& pars_v     = pars_vv[planeid];
      auto& pars_ptr_v = pars_ptr_vv[planeid];
      pars_ptr_v.reserve(pars_v.size());
      
      for(const auto& par : pars_v)
	pars_ptr_v.emplace_back(&par);
    }


    return MatchClusters(pars_ptr_vv,img_v,threshold,required_per_plane,required_matches);
    
  }
				
  std::vector<std::vector<std::pair<size_t,size_t> > >
  VertexAnalysis::MatchClusters(const std::vector<std::vector<const data::ParticleCluster* > >& pars_ptr_vv,
				const std::vector<const cv::Mat>& img_v,
				float threshold,
				size_t required_per_plane,
				size_t required_matches) {
    
    std::vector<std::vector<std::pair<size_t,size_t> > > match_vv;
    MatchExists(pars_ptr_vv,img_v,threshold,required_per_plane,required_matches,match_vv);
    return match_vv;
  }
  
  bool
  VertexAnalysis::MatchExists(const std::vector<std::vector<const data::ParticleCluster*> >& pars_ptr_vv,
			      const std::vector<const cv::Mat>& img_v,
			      float threshold,
			      size_t required_per_plane,
			      size_t required_matches,
			      std::vector<std::vector<std::pair<size_t,size_t> > >& match_vv) {


    std::vector<size_t> seed_v;
    seed_v.resize(3);

    for(size_t planeid=0;planeid<3;++planeid)
      seed_v[planeid]=pars_ptr_vv[planeid].size();

    MatchBookKeeper _MatchBookKeeper;
    _MatchBookKeeper.Reset();

    std::vector<std::vector<size_t> > clusters_per_plane_vv;
    std::vector<const data::ParticleCluster*> particle_ptr_v;
    std::vector<std::pair<size_t,size_t> > particle_id_to_plane_v;
    clusters_per_plane_vv.resize(3);

    size_t offset=0;
    for(size_t planeid=0;planeid<3;++planeid) {
      auto& clusters_per_plane_v = clusters_per_plane_vv[planeid];
      const auto& pars_v = pars_ptr_vv[planeid];
      for(size_t parid=0;parid<pars_v.size();++parid) {
	clusters_per_plane_v.push_back(parid+offset);
	particle_ptr_v.push_back(pars_v[parid]);
	particle_id_to_plane_v.emplace_back(planeid,parid);
      }
      offset+=pars_v.size();
    }

    for(const auto& comb_v : PlaneClusterCombinations(seed_v)) {
      if (comb_v.size()==2) {
	
	auto plane0=comb_v[0].first;
	auto plane1=comb_v[1].first;

	if (clusters_per_plane_vv[plane0].size()<required_per_plane) continue;
	if (clusters_per_plane_vv[plane1].size()<required_per_plane) continue;

	auto cid0=comb_v[0].second;
	auto cid1=comb_v[1].second;

	auto id0 = clusters_per_plane_vv[plane0][cid0];
	auto id1 = clusters_per_plane_vv[plane1][cid1];

	// get the paticle type
	auto type0 = particle_ptr_v[id0]->type;
	auto type1 = particle_ptr_v[id1]->type;

	// require the same type of particle to be matched
	if (type0!=type1) continue;
	
	// get the contour
	const auto& ctor0 = particle_ptr_v[id0]->_ctor;
	const auto& ctor1 = particle_ptr_v[id1]->_ctor;

	if (ctor0.empty()) throw larbys("Contour 0 @ undefined?");
	if (ctor1.empty()) throw larbys("Contour 1 @ undefined?");

	std::vector<size_t> pair_v(2,kINVALID_SIZE);

	auto img0 = MaskImage(img_v[plane0],ctor0,0,false);
	auto img1 = MaskImage(img_v[plane1],ctor1,0,false);
	
	auto nzero0_i = FindNonZero(img0);
	auto nzero1_i = FindNonZero(img1);

	//cast to float
	geo2d::VectorArray<float> nzero0_f;
	nzero0_f.reserve(nzero0_i.size());

	geo2d::VectorArray<float> nzero1_f;
	nzero1_f.reserve(nzero1_i.size());
	
	for(const auto& pt : nzero0_i)
	  nzero0_f.emplace_back(pt.x,pt.y);
	
	for(const auto& pt : nzero1_i)
	  nzero1_f.emplace_back(pt.x,pt.y);
	
	auto overlap = _geo.Overlap(nzero0_f,plane0,nzero1_f,plane1,false);

	if (overlap<threshold) continue;

	std::vector<uint> match_v(2,kINVALID_INT);
	match_v[0]=id0;
	match_v[1]=id1;
	
	_MatchBookKeeper.Match(match_v,overlap);
      }
      else if (comb_v.size()==3) {
	
	auto plane0=comb_v[0].first;
	auto plane1=comb_v[1].first;
	auto plane2=comb_v[2].first;

	if (clusters_per_plane_vv[plane0].size()<required_per_plane) continue;
	if (clusters_per_plane_vv[plane1].size()<required_per_plane) continue;
	if (clusters_per_plane_vv[plane2].size()<required_per_plane) continue;
	
	auto cid0=comb_v[0].second;
	auto cid1=comb_v[1].second;
	auto cid2=comb_v[2].second;

	auto id0 = clusters_per_plane_vv[plane0][cid0];
	auto id1 = clusters_per_plane_vv[plane1][cid1];
	auto id2 = clusters_per_plane_vv[plane2][cid2];

	// get the particle type
	auto type0 = particle_ptr_v[id0]->type;
	auto type1 = particle_ptr_v[id1]->type;
	auto type2 = particle_ptr_v[id2]->type;

	if (type0!=type1) continue;
	if (type0!=type2) continue;
	if (type1!=type2) continue;
	
	// get the contour
	const auto& ctor0 = particle_ptr_v[id0]->_ctor;
	const auto& ctor1 = particle_ptr_v[id1]->_ctor;
	const auto& ctor2 = particle_ptr_v[id2]->_ctor;

	if (ctor0.empty()) throw larbys("Contour 0 @ undefined?");
	if (ctor1.empty()) throw larbys("Contour 1 @ undefined?");
	if (ctor2.empty()) throw larbys("Contour 2 @ undefined?");

	std::vector<size_t> pair_v(3,kINVALID_SIZE);

	auto img0 = MaskImage(img_v[plane0],ctor0,0,false);
	auto img1 = MaskImage(img_v[plane1],ctor1,0,false);
	auto img2 = MaskImage(img_v[plane2],ctor2,0,false);
	
	auto nzero0_i = FindNonZero(img0);
	auto nzero1_i = FindNonZero(img1);
	auto nzero2_i = FindNonZero(img2);

	//cast to float
	geo2d::VectorArray<float> nzero0_f;
	nzero0_f.reserve(nzero0_i.size());

	geo2d::VectorArray<float> nzero1_f;
	nzero1_f.reserve(nzero1_i.size());

	geo2d::VectorArray<float> nzero2_f;
	nzero2_f.reserve(nzero2_i.size());
	
	for(const auto& pt : nzero0_i)
	  nzero0_f.emplace_back(pt.x,pt.y);

	for(const auto& pt : nzero1_i)
	  nzero1_f.emplace_back(pt.x,pt.y);

	for(const auto& pt : nzero2_i)
	  nzero2_f.emplace_back(pt.x,pt.y);
	
	auto overlap01 = _geo.Overlap(nzero0_f,plane0,nzero1_f,plane1,false);
	auto overlap02 = _geo.Overlap(nzero0_f,plane0,nzero2_f,plane2,false);
	auto overlap12 = _geo.Overlap(nzero1_f,plane1,nzero2_f,plane2,false);

	ushort invalid_overlap_count=0;

	if (overlap01<threshold) invalid_overlap_count+=1;
	if (overlap02<threshold) invalid_overlap_count+=1;
	if (overlap12<threshold) invalid_overlap_count+=1;

	if (invalid_overlap_count >= 2) continue;

	auto overlap = overlap01+overlap02+overlap12;
	if (overlap == 0.0) continue;
	
	std::vector<uint> match_v(3,kINVALID_INT);
	match_v[0]=id0;
	match_v[1]=id1;
	match_v[2]=id2;
	
	_MatchBookKeeper.Match(match_v,overlap);
      }

      else {
	throw larbys("Observed unhandled combination in algorithm");
      }

    } // end combos

    LAROCV_DEBUG() << "Found " << _MatchBookKeeper.GetResult().size() << " particle matches" << std::endl;
    
    if ( _MatchBookKeeper.GetResult().size() < required_matches )
      return false;
    
    match_vv.reserve(_MatchBookKeeper.GetResult().size());
    
    for(const auto& match : _MatchBookKeeper.GetResult()) {
      std::vector<std::pair<size_t,size_t> > match_v;
      match_v.reserve(match.size()); // the number of matches contours
      LAROCV_DEBUG() << "... matched " << match.size() << " particles" << std::endl;
      for(auto par_id : match)
	match_v.emplace_back(particle_id_to_plane_v[par_id]);
      match_vv.emplace_back(std::move(match_v));
    }
      
    return true;
  }

  bool
  VertexAnalysis::RequireParticleCount(const std::vector<std::vector<data::ParticleCluster> >& pars_vv,
				       uint nplanes,
				       uint nxs) {
    // For now I have to make a pointer
    std::vector<std::vector<const data::ParticleCluster*> > pars_ptr_vv;
    pars_ptr_vv.resize(pars_vv.size());
    
    for(size_t planeid=0;planeid<pars_vv.size();++planeid) {
      auto& pars_v     = pars_vv[planeid];
      auto& pars_ptr_v = pars_ptr_vv[planeid];
      pars_ptr_v.reserve(pars_v.size());
      
      for(const auto& par : pars_v)
	pars_ptr_v.emplace_back(&par);
    }

    return RequireParticleCount(pars_ptr_vv,nplanes,nxs);
  }
  
  bool
  VertexAnalysis::RequireParticleCount(const std::vector<std::vector<const data::ParticleCluster*> >& pars_ptr_vv,
				       uint nplanes,
				       uint nxs) {
    if (pars_ptr_vv.size()!=3)
      throw larbys("Input particle array should be of size 3");

    uint valid_planes=0;
    for(size_t planeid=0;planeid<3;++planeid)
      if (pars_ptr_vv[planeid].size()==nxs)
	valid_planes++;

    if (valid_planes<nplanes) return false;
    
    return true;
  }

  bool
  VertexAnalysis::RequireCrossing(const data::Vertex3D& vtx3d,
				  uint nplanes,
				  uint nxs) {
    uint nplanes_=0;
    for(size_t plane=0;plane<vtx3d.vtx2d_v.size();++plane) {
      const auto& cvtx = vtx3d.cvtx2d_v.at(plane);
      if (cvtx.xs_v.size() != nxs) continue;
      nplanes_++;
    }
    if (nplanes_<nplanes) return false;

    return true;
  }

  void
  VertexAnalysis::FilterByCrossing(std::vector<const data::Vertex3D*>& vtx_v,
				   uint nplanes,
				   uint nxs) {

    std::vector<const data::Vertex3D*> vtx_temp_v;
    for(const auto vtx3d : vtx_v) {
      if ( !RequireCrossing(*vtx3d,nplanes,nxs) ) continue;
      vtx_temp_v.push_back(vtx3d);
    }

    std::swap(vtx_temp_v,vtx_v);
  }
  
}

#endif