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

//#include <typeinfo>

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
  }

  void ElectronShowerVertexSeed::SetPlaneInfo(const ImageMeta& meta)
  { _geo_algo.ResetPlaneInfo(meta); }

  void ElectronShowerVertexSeed::RegisterSeed(const larocv::data::LinearTrackArray& data)
  {
    for(auto const& ltrack : data.get_clusters()) {

      if(ltrack.edge1.vtx2d_v.size())
	_ltrack_vertex_v.push_back(ltrack.edge1);
      if(ltrack.edge2.vtx2d_v.size())
	_ltrack_vertex_v.push_back(ltrack.edge2);
    }
    LAROCV_INFO() << "# Vertex3D from LinearCluster: " << _ltrack_vertex_v.size() << std::endl;
  }

  void ElectronShowerVertexSeed::RegisterSeed(const std::vector<const cv::Mat>& img_v,
					      const larocv::data::VertexClusterArray& part,
					      const larocv::data::dQdXProfilerData& dqdx)
  {
    std::vector<bool> good_plane_v(_num_planes,false);
    std::multimap<float,larocv::data::Vertex3D> vtrack_vtx_cand_m;
    std::multimap<float,larocv::data::Vertex3D> vedge_vtx_cand_m;
    for(auto const& vtx_data : part._vtx_cluster_v) {

      for(size_t plane=0; plane<good_plane_v.size(); ++plane)
	good_plane_v[plane] = false;
      
      size_t num_good_planes = 0;
      float  pxfrac_sum = 0;
      for(size_t plane=0; plane<_num_planes; ++plane) {
	float pxfrac = vtx_data.num_pixel_fraction(plane);
	if(pxfrac < _part_pxfrac_threshold) continue;
	pxfrac_sum += pxfrac;
	good_plane_v[plane] = true;
	++num_good_planes;
      }
      if(num_good_planes < 2) continue;

      // Fill a candidate interaction vertex
      vtrack_vtx_cand_m.emplace(pxfrac_sum,vtx_data.get_vertex());
      
      auto const& partarray = dqdx.get_vertex_cluster(vtx_data.id());

      // Next fill edge-vertex
      for(size_t plane1=0; plane1<_num_planes; ++plane1) {
	if(!good_plane_v[plane1]) continue;
	for(size_t plane2=plane1+1; plane2<_num_planes; ++plane2) {
	  if(!good_plane_v[plane2]) continue;

	  std::vector<geo2d::Vector<float> > edge1_v;
	  for(auto const& part : partarray.get_cluster(plane1))
	    edge1_v.push_back(part.end_pt());
	  
	  std::vector<geo2d::Vector<float> > edge2_v;
	  for(auto const& part : partarray.get_cluster(plane2))
	    edge2_v.push_back(part.end_pt());

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

		// if ( (min_x + _circle_default_radius+0.5) > mat.cols ) throw larbys("scanning past image col boundary");
		// if ( (min_y + _circle_default_radius+0.5) > mat.rows ) throw larbys("scanning past image row boundary");

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
    LAROCV_INFO() << "# Vertex3D from dQdXProfiler: " << _vedge_vertex_v.size() << std::endl;
  }

  void ElectronShowerVertexSeed::RegisterSeed(const std::vector<const cv::Mat>& img_v,
					      const larocv::data::VertexClusterArray& part)
  {
    std::vector<bool> good_plane_v(_num_planes,false);
    std::multimap<float,larocv::data::Vertex3D> vtrack_vtx_cand_m;
    std::multimap<float,larocv::data::Vertex3D> vedge_vtx_cand_m;
    for(auto const& vtx_data : part._vtx_cluster_v) {

      for(size_t plane=0; plane<good_plane_v.size(); ++plane)
	good_plane_v[plane] = false;
      
      size_t num_good_planes = 0;
      float  pxfrac_sum = 0;
      for(size_t plane=0; plane<_num_planes; ++plane) {
	float pxfrac = vtx_data.num_pixel_fraction(plane);
	if(pxfrac < _part_pxfrac_threshold) continue;
	pxfrac_sum += pxfrac;
	good_plane_v[plane] = true;
	++num_good_planes;
      }
      if(num_good_planes < 2) continue;

      // Fill a candidate interaction vertex
      vtrack_vtx_cand_m.emplace(pxfrac_sum,vtx_data.get_vertex());

      // Next fill edge-vertex
      for(size_t plane1=0; plane1<_num_planes; ++plane1) {
	if(!good_plane_v[plane1]) continue;
	for(size_t plane2=plane1+1; plane2<_num_planes; ++plane2) {
	  if(!good_plane_v[plane2]) continue;

	  std::vector<geo2d::Vector<float> > edge1_v;
	  for(auto const& part : vtx_data.get_clusters(plane1))
	    edge1_v.push_back(part.end_pt());
	  
	  std::vector<geo2d::Vector<float> > edge2_v;
	  for(auto const& part : vtx_data.get_clusters(plane2))
	    edge2_v.push_back(part.end_pt());

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

		// if ( (min_x + _circle_default_radius+0.5) > mat.cols ) throw larbys("scanning past image col boundary");
		// if ( (min_y + _circle_default_radius+0.5) > mat.rows ) throw larbys("scanning past image row boundary");

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
    LAROCV_INFO() << "# Vertex3D from dQdXProfiler: " << _vedge_vertex_v.size() << std::endl;
  }
  
  std::vector<larocv::data::Vertex3D>
  ElectronShowerVertexSeed::ListCandidateVertex(const std::vector<larocv::data::Vertex3D>& ltrack_vertex_v,
						const std::vector<larocv::data::Vertex3D>& vtrack_vertex_v,
						const std::vector<larocv::data::Vertex3D>& vedge_vertex_v) const
  {
    std::vector<larocv::data::Vertex3D> res_v;
    for(auto const& pt : ltrack_vertex_v) {
      float min_dist = 1e9;
      for(auto const& vtx : res_v) {
	float dist = sqrt(pow(vtx.x-pt.x,2)+pow(vtx.y-pt.y,2)+pow(vtx.z-pt.z,2));
	if(dist < min_dist) min_dist = dist;
	if(min_dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) {
	LAROCV_INFO() << "Skipping LinearTrack vertex @ ("<<pt.x<<","<<pt.y<<","<<pt.z<<")"<<std::endl;
	continue;
      }
      res_v.push_back(pt);
    }
    for(auto const& pt : vtrack_vertex_v) {
      float min_dist = 1e9;
      for(auto const& vtx : res_v) {
	float dist = sqrt(pow(vtx.x-pt.x,2)+pow(vtx.y-pt.y,2)+pow(vtx.z-pt.z,2));
	if(dist < min_dist) min_dist = dist;
	if(min_dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) {
	LAROCV_INFO() << "Skipping VertexTrackCluster vertex @ ("<<pt.x<<","<<pt.y<<","<<pt.z<<")"<<std::endl;
	continue;
      }
      res_v.push_back(pt);
    }
    for(auto const& pt : vedge_vertex_v) {
      float min_dist = 1e9;
      for(auto const& vtx : res_v) {
	float dist = sqrt(pow(vtx.x-pt.x,2)+pow(vtx.y-pt.y,2)+pow(vtx.z-pt.z,2));
	if(dist < min_dist) min_dist = dist;
	if(min_dist < _vertex_min_separation) break;
      }
      if(min_dist < _vertex_min_separation) {
	LAROCV_INFO() << "Skipping ParticleEnd vertex @ ("<<pt.x<<","<<pt.y<<","<<pt.z<<")"<<std::endl;
	continue;
      }
      res_v.push_back(pt);
    }
    return res_v;
  }

  std::vector<data::Vertex3D>
  ElectronShowerVertexSeed::CreateSeed()
  { return ListCandidateVertex(_ltrack_vertex_v,_vtrack_vertex_v,_vedge_vertex_v); }

}
#endif
