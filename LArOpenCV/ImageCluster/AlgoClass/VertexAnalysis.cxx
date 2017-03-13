#ifndef __VERTEXANALYSIS_CXX__
#define __VERTEXANALYSIS_CXX__

#include "VertexAnalysis.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/Base/BaseUtil.h"
#include "LArOpenCV/ImageCluster/Base/MatchBookKeeper.h"

using larocv::larbys;

namespace larocv {

  VertexAnalysis::VertexAnalysis()
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


  bool
  VertexAnalysis::MatchExists(std::vector<std::vector<data::ParticleCluster> >& pars_vv,
			      const cv::Mat& img,
			      float threshold,
			      size_t required_per_plane) {


    std::vector<size_t> seed_v;
    seed_v.resize(3);

    for(size_t planeid=0;planeid<3;++planeid)
      seed_v[planeid]=pars_vv[planeid].size();

    MatchBookKeeper _MatchBookKeeper;

    std::vector<std::vector<size_t> > clusters_per_plane_vv;
    std::vector<const GEO2D_Contour_t*> clusters_ctor_v;
    
    clusters_per_plane_vv.resize(3);

    size_t offset=0;
    for(size_t planeid=0;planeid<3;++planeid) {
      auto& clusters_per_plane_v = clusters_per_plane_vv[planeid];
      const auto& pars_v = pars_vv[planeid];
      for(size_t parid=0;parid<pars_v.size();++parid) {
	clusters_per_plane_v.push_back(parid+offset);
	clusters_ctor_v.push_back(&pars_v[parid]._ctor);
      }
      offset+=pars_v.size();
    }

    for(const auto& comb_v : PlaneClusterCombinations(seed_v)) {
      if (comb_v.size()==2) {}
      if (comb_v.size()==3) {}
    }

    return true;
	  
  }

  
  bool
  VertexAnalysis::RequireParticleCount(std::vector<std::vector<data::ParticleCluster> >& pars_vv,
				       uint nplanes,
				       uint nxs) {
    if (pars_vv.size()!=3)
      throw larbys("Input particle array should be of size 3");

    uint valid_planes=0;
    for(size_t planeid=0;planeid<3;++planeid)
      if (pars_vv[planeid].size()==nxs)
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
