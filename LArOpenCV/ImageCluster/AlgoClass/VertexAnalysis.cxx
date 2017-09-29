#ifndef __VERTEXANALYSIS_CXX__
#define __VERTEXANALYSIS_CXX__

#include "VertexAnalysis.h"
#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#endif
#endif
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

using larocv::larbys;

namespace larocv {

  VertexAnalysis::VertexAnalysis() : _geo()
  {}

  void
  VertexAnalysis::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)pset.get<int>("Verbosity"));
  }

  void
  VertexAnalysis::ResetGeo() {
    _geo.Reset();
  }
  
  void
  VertexAnalysis::ResetPlaneInfo(const larocv::ImageMeta& meta)
  {
    _geo.ResetPlaneInfo(meta);
  }

  void
  VertexAnalysis::MergeNearby(const std::vector<const data::Vertex3D*>& vtx1_v,
			      std::vector<const data::Vertex3D*>& vtx2_v,
			      double dist3d) const {

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
  VertexAnalysis::MergeNearby(std::vector<data::Vertex3D>& vtx_v,
			      double dist3d) const {

    std::vector<data::Vertex3D> vtx_tmp_v = vtx_v;
    std::vector<data::Vertex3D> vtx_res_v;
    
    std::vector<bool> merged_v;
    float min_dist;
    size_t closeid;
    bool close;
    
    for(;;) {

      close = false;
      
      merged_v.resize(vtx_tmp_v.size(),false);
      vtx_res_v.reserve(vtx_tmp_v.size());
			
      for(size_t v1=0;v1<vtx_tmp_v.size();++v1) {
	if (merged_v[v1]) continue;
	const auto& vtx1 = vtx_tmp_v[v1];
	min_dist=kINVALID_FLOAT;
	closeid =kINVALID_SIZE;
	for(size_t v2=v1+1;v2<vtx_tmp_v.size();++v2) {
	  if (merged_v[v2]) continue;
	  const auto& vtx2 = vtx_tmp_v[v2];
	  auto dist = data::Distance(vtx1,vtx2);
	  if (dist < min_dist) {
	    min_dist = dist;
	    closeid  = v2;
	  }
	}

	if (min_dist > dist3d) continue;

	close = true;

	if (min_dist==kINVALID_FLOAT)throw larbys("logic error");
	if (closeid ==kINVALID_SIZE) throw larbys("logic error");
	  
	merged_v[v1]      = true;
	merged_v[closeid] = true;
	vtx_res_v.emplace_back(AverageVertex(vtx1,vtx_tmp_v[closeid]));
      }

      for(size_t vtxid=0;vtxid<merged_v.size();++vtxid) {
	if (merged_v[vtxid]) continue;
	vtx_res_v.emplace_back(std::move(vtx_tmp_v[vtxid]));
      }
      
      std::swap(vtx_tmp_v,vtx_res_v);
      vtx_res_v.clear();
      if(!close) break;
    }
    
    std::swap(vtx_tmp_v,vtx_v);
  }
  
  bool
  VertexAnalysis::MatchEdge(const std::array<const data::TrackClusterCompound*,3>& tcluster_arr,
			    data::Vertex3D& vertex) const {
    short ntracks=0;
    for(auto tcluster_ptr : tcluster_arr)
      if(tcluster_ptr)
	ntracks++;

    if (ntracks<2) {
      LAROCV_WARNING() << "Provided number of tracks less than 2" << std::endl;
      return false;
    }
    
    if (ntracks==2) {
      size_t plane0=kINVALID_SIZE;
      size_t plane1=kINVALID_SIZE;

      for(size_t plane=0;plane<3;++plane) {
	if(tcluster_arr[plane]) {
	  if(plane0==kINVALID_SIZE) {
	    plane0=plane;
	  }
	  else if(plane1==kINVALID_SIZE) {
	    plane1=plane;
	  }
	}
      }
      
      if (plane0==kINVALID_SIZE) throw larbys("Logic error plane0");
      if (plane1==kINVALID_SIZE) throw larbys("Logic error plane1");
      if (plane0==plane1)        throw larbys("Logic error plane0 and plane1");

      auto& track0 = *(tcluster_arr[plane0]);
      auto& track1 = *(tcluster_arr[plane1]);
      
      auto endok = MatchEdge(track0,plane0,track1,plane1,vertex);
      return endok;
    }
    else if (ntracks==3) {
      auto& track0  = *(tcluster_arr[0]);
      auto& track1  = *(tcluster_arr[1]);
      auto& track2  = *(tcluster_arr[2]);
      size_t plane0 = 0;
      size_t plane1 = 1;
      size_t plane2 = 2;
      
      auto endok = MatchEdge(track0,plane0,track1,plane1,track2,plane2,vertex);
      return endok;
    }

    throw larbys("Logic error");
    return false;
  }
  

  bool
  VertexAnalysis::MatchEdge(const data::TrackClusterCompound& track0, size_t plane0,
			    const data::TrackClusterCompound& track1, size_t plane1,
			    data::Vertex3D& vertex) const {

    bool endok;
    auto end0 = track0.end_pt();
    auto end1 = track1.end_pt();
    endok = _geo.YZPoint(end0,plane0,end1,plane1,vertex);
    return endok;
  }

  bool
  VertexAnalysis::MatchEdge(const data::TrackClusterCompound& track0, size_t plane0,
			    const data::TrackClusterCompound& track1, size_t plane1,
			    const data::TrackClusterCompound& track2, size_t plane2,
			    data::Vertex3D& vertex) const {
    
    bool endok;

    auto end0 = track0.end_pt();
    auto end1 = track1.end_pt();
    auto end2 = track2.end_pt();
    
    LAROCV_DEBUG() << "Testing end0 @ " << end0 << " on plane " << plane0 << " & end1 " << end1 << " @ plane " << plane1 << std::endl;
    endok = _geo.YZPoint(end0,plane0,end1,plane1,vertex);
    if (!endok) { 
      LAROCV_DEBUG() << "Testing end0 @ " << end0 << " on plane " << plane0 << " & end2 " << end2 << " @ plane " << plane2 << std::endl;
      endok = _geo.YZPoint(end0,plane0,end2,plane2,vertex);
    }
    if (!endok) {
      LAROCV_DEBUG() << "Testing end1 @ " << end1 << " on plane " << plane1 << " & end2 " << end2 << " @ plane " << plane2 << std::endl;
      endok = _geo.YZPoint(end1,plane1,end2,plane2,vertex);
    }
    
    return endok;
  }
  
  
  bool
  VertexAnalysis::RequireParticleCount(const std::vector<std::vector<data::ParticleCluster> >& pars_vv,
				       uint nplanes,
				       uint nxs) const {
    // For now I have to make a pointer
    std::vector<std::vector<const data::ParticleCluster*> > pars_ptr_vv;
    pars_ptr_vv.resize(pars_vv.size());
    
    for(size_t plane=0;plane<pars_vv.size();++plane) {
      auto& pars_v     = pars_vv[plane];
      auto& pars_ptr_v = pars_ptr_vv[plane];
      pars_ptr_v.reserve(pars_v.size());
      
      for(const auto& par : pars_v)
	pars_ptr_v.emplace_back(&par);
    }

    return RequireParticleCount(pars_ptr_vv,nplanes,nxs);
  }
  
  bool
  VertexAnalysis::RequireParticleCount(const std::vector<std::vector<const data::ParticleCluster*> >& pars_ptr_vv,
				       uint nplanes,
				       uint nxs) const {
    if (pars_ptr_vv.size()!=3)
      throw larbys("Input particle array should be of size 3");

    uint valid_planes=0;
    for(size_t plane=0;plane<3;++plane)
      if (pars_ptr_vv[plane].size()==nxs)
	valid_planes++;

    if (valid_planes<nplanes) return false;
    
    return true;
  }

  bool
  VertexAnalysis::RequireCrossing(const data::Vertex3D& vtx3d,
				  uint nplanes,
				  uint nxs) const {
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
				   uint nxs) const {

    std::vector<const data::Vertex3D*> vtx_temp_v;
    for(const auto vtx3d : vtx_v) {
      if ( !RequireCrossing(*vtx3d,nplanes,nxs) ) continue;
      vtx_temp_v.push_back(vtx3d);
    }

    std::swap(vtx_temp_v,vtx_v);
  }


  // from jarrett
  bool
  VertexAnalysis::CheckFiducial(const data::Vertex3D& vtx,
				float edge_x, float edge_y, float edge_z) const {

    auto X = vtx.x;
    auto Y = vtx.y;
    auto Z = vtx.z;
    
    static float xmin =  0.0;
    static float ymin = -116.5;
    static float zmin =  0.0;

    static float xmax =  256.25; //cm
    static float ymax =  116.5;  //cm
    static float zmax = 1036.8;  //cm

    bool XInFid = (X < (xmax - edge_x) && (X > xmin + edge_x) );
    bool YInFid = (Y < (ymax - edge_y) && (Y > ymin + edge_y) );
    bool ZInFid = (Z < (zmax - edge_z) && (Z > zmin + edge_z) );

    if (XInFid && YInFid && ZInFid) return true;

    return false;
  }


  data::Vertex3D
  VertexAnalysis::AverageVertex(const data::Vertex3D& vtx1, const data::Vertex3D& vtx2) const {
    data::Vertex3D res;
    res.x = (vtx1.x + vtx2.x)/2.0;
    res.y = (vtx1.y + vtx2.y)/2.0;
    res.z = (vtx1.z + vtx2.z)/2.0;
      
    res.type = vtx1.type;
    return res;
  }
  
  bool
  VertexAnalysis::UpdatePlanePosition(data::Vertex3D& vtx, const LArPlaneGeo& geo) const {

    vtx.vtx2d_v.clear();
    vtx.vtx2d_v.resize(3);
    for(size_t plane=0; plane<3; ++plane) {
      auto& vtx2d = vtx.vtx2d_v[plane].pt;
      vtx2d.x = geo.x2col (vtx.x,plane);
      vtx2d.y = geo.yz2row(vtx.y,vtx.z,plane);
    }
    
    return true;
  }

  bool
  VertexAnalysis::UpdatePlanePosition(data::Vertex3D& vtx) const {
    return UpdatePlanePosition(vtx,_geo);
  }

  bool
  VertexAnalysis::UpdatePlanePosition(data::Vertex3D& vtx, const LArPlaneGeo& geo, size_t plane) const {

    if (vtx.vtx2d_v.size() <= plane)
      vtx.vtx2d_v.resize(plane+1);

    if (vtx.vtx2d_v.size() > 3) throw larbys("Resize error");
    
    auto& vtx2d = vtx.vtx2d_v.at(plane).pt;
    vtx2d.x = geo.x2col (vtx.x,plane);
    vtx2d.y = geo.yz2row(vtx.y,vtx.z,plane);
    
    return true;
  }

  
  bool
  VertexAnalysis::UpdatePlanePosition(data::Vertex3D& vtx, const LArPlaneGeo& geo, std::array<bool,3>& in_image_v) const {

    vtx.vtx2d_v.clear();
    vtx.vtx2d_v.resize(3);
    for(size_t plane=0; plane<3; ++plane) {
      auto& vtx2d = vtx.vtx2d_v[plane].pt;
      in_image_v[plane] = true;
      try{ 
	vtx2d.x = geo.x2col (vtx.x,plane);
	vtx2d.y = geo.yz2row(vtx.y,vtx.z,plane);
      }
      catch(const larbys& err){
	in_image_v[plane] = false;
	continue;
      }
    }
    
    return true;
  }
  
}

#endif
