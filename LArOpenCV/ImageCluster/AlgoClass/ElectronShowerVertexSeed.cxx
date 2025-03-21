#ifndef __ELECTRONSHOWERVERTEXSEED_CXX__
#define __ELECTRONSHOWERVERTEXSEED_CXX__

#include "Geo2D/Core/VectorArray.h"
#include "Geo2D/Core/Geo2D.h"
#include "Geo2D/Core/Line.h"
#include "ElectronShowerVertexSeed.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/SpectrumAnalysis.h"

namespace larocv {
  
  void ElectronShowerVertexSeed::Reset()
  {
    _geo_algo.Reset();
    _geo_algo._xplane_tick_resolution = _xplane_tick_resolution;
    _geo_algo._num_planes = _num_planes;
    _ltrack_vertex_v.clear();
    _vtrack_vertex_v.clear();
    _vedge_vertex_v.clear();
  }

  void ElectronShowerVertexSeed::Configure(const Config_t &pset)
  {
    _part_pxfrac_threshold = pset.get<float>("PixelFracThreshold");
    _circle_default_radius = pset.get<float>("CircleDefaultRadius",10);//10

    _pi_threshold = 5;
    _vertex_min_separation = 3;
    _xplane_tick_resolution = 3;
    _num_planes = 3;

    _geo_algo.Configure(pset.get<Config_t>("LArPlaneGeo"));
  }

  void ElectronShowerVertexSeed::SetPlaneInfo(const ImageMeta& meta)
  { _geo_algo.ResetPlaneInfo(meta); }

  void ElectronShowerVertexSeed::RegisterSeed(const larocv::data::LinearTrack3DArray& data)
  {

    for (const auto& ltrack : data.as_vector()) { 
      if(ltrack.edge1.vtx2d_v.size()) {
	_ltrack_vertex_v.push_back(ltrack.edge1);
      }
      if(ltrack.edge2.vtx2d_v.size()) {
	_ltrack_vertex_v.push_back(ltrack.edge2);
      }
    }
    
    LAROCV_INFO() << "# Vertex3D from LinearCluster: " << _ltrack_vertex_v.size() << std::endl;
  }
  
  void ElectronShowerVertexSeed::RegisterSeed(const std::vector<cv::Mat>& img_v,
					      const std::vector<data::VertexTrackInfoCollection>& vtxinfo_v)
  {

    std::vector<bool> good_plane_v(_num_planes,false);
    std::multimap<float,larocv::data::Vertex3D> vtrack_vtx_cand_m;
    std::multimap<float,larocv::data::Vertex3D> vedge_vtx_cand_m;

    LAROCV_INFO() << "Attempting register of " << vtxinfo_v.size() << " track vertex infos" << std::endl;
    for(auto const& vtxinfo : vtxinfo_v) {

      for(size_t plane_id=0; plane_id<good_plane_v.size(); ++plane_id)
	good_plane_v[plane_id] = false;
      
      // Get useful attrivute references
      auto const& super_contour_v = vtxinfo.super_contour_v;
      auto const& particle_vv = vtxinfo.particle_vv;
      auto const& compound_vv = vtxinfo.compound_vv;

      if(super_contour_v.size() != _num_planes) {
	LAROCV_CRITICAL() << "# planes for super cluster incorrect..." << std::endl;
	throw larbys();
      }
      if(particle_vv.size() != _num_planes) {
	LAROCV_CRITICAL() << "# planes for ParticleClusterArray incorrect..." << std::endl;
	throw larbys();
      }
      if(compound_vv.size() != _num_planes) {
	LAROCV_CRITICAL() << "# planes for CompoundClusterArray incorrect..." << std::endl;
	throw larbys();
      }
      size_t num_good_planes = 0;
      float  pxfrac_sum = 0;
      for(size_t plane=0; plane<_num_planes; ++plane) {
	double pxfrac = 0;
	for(const auto particle : particle_vv.at(plane)) {
	  auto pxfrac_ = PixelFraction(img_v[plane],super_contour_v.at(plane)->_ctor,particle->_ctor);
	  pxfrac+=pxfrac_;
	}
	LAROCV_DEBUG() << "Final px fraction is " << pxfrac << std::endl;
	if(pxfrac < _part_pxfrac_threshold) continue;
	pxfrac_sum += pxfrac;
	good_plane_v[plane] = true;
	++num_good_planes;
      }
      if(num_good_planes < 2) continue;

      LAROCV_INFO() << "Inspecting track vertex seed @ (" << vtxinfo.vtx3d->x << "," << vtxinfo.vtx3d->y << "," << vtxinfo.vtx3d->z << ")" << std::endl;
      if (vtxinfo.vtx3d->vtx2d_v.size() != _num_planes) {
	LAROCV_CRITICAL() << "... this vertex num planes " << vtxinfo.vtx3d->vtx2d_v.size() << " is invalid" << std::endl;
	throw larbys();
      }
      data::Vertex3D vtx3d = *(vtxinfo.vtx3d);
      
      vtrack_vtx_cand_m.emplace(pxfrac_sum,vtx3d);
      
      // Next fill edge-vertex
      for(size_t plane1=0; plane1<_num_planes; ++plane1) {
	if(!good_plane_v[plane1]) continue;
	for(size_t plane2=plane1+1; plane2<_num_planes; ++plane2) {
	  if(!good_plane_v[plane2]) continue;
	  
	  std::vector<geo2d::Vector<float> > edge1_v;
	  for(auto const& compound : compound_vv[plane1])
	    edge1_v.push_back(compound->end_pt());
	  
	  std::vector<geo2d::Vector<float> > edge2_v;
	  for(auto const& compound : compound_vv[plane2])
	    edge2_v.push_back(compound->end_pt());
	  
	  // Loop over possible combinations
	  larocv::data::Vertex3D pt;
	  for(auto const& edge1 : edge1_v) {
	    for(auto const& edge2 : edge2_v) {

	      if(!_geo_algo.YZPoint(edge1,plane1,edge2,plane2,pt))
		continue;

	      float check_dist_min = _circle_default_radius;
	      for(size_t check_plane=0; check_plane<_num_planes; ++check_plane) {
		if(check_plane == plane1) continue;
		if(check_plane == plane2) continue;
		auto const& check_pt = pt.vtx2d_v[check_plane].pt;
		auto const& mat = img_v[check_plane];
		size_t min_x = (size_t)(check_pt.x - _circle_default_radius/2. + 0.5);
		size_t min_y = (size_t)(check_pt.y - _circle_default_radius/2. + 0.5);
		size_t step_x,step_y;
		for(size_t dx=0; dx<(size_t)(_circle_default_radius+0.5); ++dx) {
		  step_x = min_x+dx;
		  if ( step_x >= mat.cols )
		    { LAROCV_INFO() << "Skipping step outside img col bound"<<std::endl; continue; }
		  for(size_t dy=0; dy<(size_t)(_circle_default_radius+0.5); ++dy) {
		    step_y = min_y+dy;
		    if ( step_y >= mat.rows )
		      { LAROCV_INFO() << "Skipping step outside img row bound"<<std::endl; continue; }
		    if(mat.at<unsigned char>(step_y,step_x) < _pi_threshold) continue;
		    float dist = sqrt(pow((min_x+dx)-check_pt.x,2) + pow((min_y+dy)-check_pt.y,2));
		    if(dist < check_dist_min) check_dist_min = dist;
		  }
		}
	      }
	      vedge_vtx_cand_m.emplace(check_dist_min, pt);
	    }
	  }
	}
      }
    }
    
    _vedge_vertex_v.clear();
    for(auto const& score_vtx : vedge_vtx_cand_m) {
      // check distance to the previous ... needs to be at least required 3D separation
      auto const& cand_vtx = score_vtx.second;
      float min_dist=1e9;
      for(auto const& vedge_vtx : _vedge_vertex_v) {
	float dist = sqrt(pow(vedge_vtx.x - cand_vtx.x,2) +
			  pow(vedge_vtx.y - cand_vtx.y,2) +
			  pow(vedge_vtx.z - cand_vtx.z,2));
	if(dist < min_dist) min_dist = dist;
	if(dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) continue;
      _vedge_vertex_v.push_back(cand_vtx);
    }

    _vtrack_vertex_v.clear();
    for(auto const& score_vtx : vtrack_vtx_cand_m) {
      // check distance to the previous ... needs to be at least required 3D separation
      auto const& cand_vtx = score_vtx.second;
      float min_dist=1e9;
      for(auto const& vedge_vtx : _vtrack_vertex_v) {
	float dist = sqrt(pow(vedge_vtx.x - cand_vtx.x,2) +
			  pow(vedge_vtx.y - cand_vtx.y,2) +
			  pow(vedge_vtx.z - cand_vtx.z,2));
	if(dist < min_dist) min_dist = dist;
	if(dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) continue;
      _vtrack_vertex_v.push_back(cand_vtx);
    }

    LAROCV_INFO() << "# Vertex3D from VertexTrackCluster: " << _vtrack_vertex_v.size() << std::endl;
    LAROCV_INFO() << "# Vertex3D from ParticleCluster: " << _vedge_vertex_v.size() << std::endl;
  }
  
  void
  ElectronShowerVertexSeed::ListCandidateVertex(std::vector<larocv::data::VertexSeed3D>& res_v,
						const std::vector<larocv::data::Vertex3D>& vertex_v,
						data::SeedType_t type) 
  {
    for (const auto& pt : vertex_v){
      float min_dist = 1e9;
      for(auto const& vtx : res_v) {
	float dist = sqrt(pow(vtx.x-pt.x,2)+pow(vtx.y-pt.y,2)+pow(vtx.z-pt.z,2));
	if(dist < min_dist) min_dist = dist;
	if(min_dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) {
	LAROCV_INFO() << "Skipping vertex type " << (uint)type << " @ ("<<pt.x<<","<<pt.y<<","<<pt.z<<")"<<std::endl;
	continue;
      }
      data::VertexSeed3D seed(pt);
      seed.type=type;
      res_v.emplace_back(std::move(seed));
    }
  }

  std::vector<data::VertexSeed3D>
  ElectronShowerVertexSeed::CreateSeed()
  {
    std::vector<larocv::data::VertexSeed3D> res_v;
    res_v.reserve(10); // resreve space for 10 seeds
    ListCandidateVertex(res_v,_ltrack_vertex_v,data::SeedType_t::kEdge);
    ListCandidateVertex(res_v,_vtrack_vertex_v,data::SeedType_t::kTrack);
    ListCandidateVertex(res_v,_vedge_vertex_v,data::SeedType_t::kEnd);
    return res_v;
  }
}
#endif
