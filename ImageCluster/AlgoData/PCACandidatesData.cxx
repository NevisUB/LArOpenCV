#ifndef __ALGODATA_PCACANDIDATESDATA_CXX__
#define __ALGODATA_PCACANDIDATESDATA_CXX__

#include "PCACandidatesData.h"
#include "Core/larbys.h"

namespace larocv {

  namespace data {
    
    void PCACandidatesData::Clear() {

      _input_id = kINVALID_ALGO_ID;
      _line_v.clear();      
      _points_vv.clear();

      _input_id = kINVALID_ID;
      _reverse_index_map.clear();
      _index_map.clear();
    }

    void PCACandidatesData::insert(const geo2d::Line<float>& line,
				   size_t atom_id, size_t cluster_id, size_t plane_id, size_t vtx_id)
    {
      insert_id(atom_id, cluster_id, plane_id, vtx_id, _line_v.size());
      _line_v.push_back(line);
    }

    void PCACandidatesData::move(geo2d::Line<float>&& line,
				 size_t atom_id, size_t cluster_id, size_t plane_id, size_t vtx_id)
    {
      insert_id(atom_id, cluster_id, plane_id, vtx_id, _line_v.size());
      _line_v.emplace_back(std::move(line));
    }

    void PCACandidatesData::insert_id(size_t atom_id, size_t cluster_id, size_t plane_id, size_t vtx_id, size_t pca_id) 
    {
      if(plane_id >= 3) throw larbys("Invalid plane id provided!");

      VertexPlaneClusterAtomicMap id;
      _reverse_index_map.resize(pca_id+1,id);

      id.vertex_id  = vtx_id;
      id.plane_id   = plane_id;
      id.cluster_id = cluster_id;
      id.atomic_id  = atom_id;
      _reverse_index_map[pca_id] = id;

      // If this has no associated vertx, store @ index 0
      if(vtx_id==kINVALID_SIZE) vtx_id = 0;
      
      if(_index_map.size() <= vtx_id) {
	_index_map.resize(vtx_id+1);
	for(auto& plane : _index_map)
	  if(plane.size() != 3) plane.resize(3);
      }
      auto& plane = _index_map[vtx_id][plane_id];
      if(plane.size() <= cluster_id) plane.resize(cluster_id+1);
      auto& cluster = plane[cluster_id];
      if(cluster.size() <= atom_id) cluster.resize(atom_id+1,kINVALID_SIZE);
      cluster[atom_id] = pca_id;
    }

    const VertexPlaneClusterAtomicMap& PCACandidatesData::parent_id(size_t idx) const
    {
      if(idx >= _reverse_index_map.size()) throw larbys("Requested an invalid PCA line/points id!");
      return _reverse_index_map[idx];
    }

    size_t PCACandidatesData::index_atom(size_t atom_id, size_t cluster_id, size_t plane_id, size_t vtx_id) const
    {
      if(vtx_id == kINVALID_SIZE) vtx_id = 0;
      if(vtx_id >= _index_map.size()) throw larbys("Invalid vertex ID requested!");
      auto& vtx = _index_map[vtx_id];
      if(plane_id >= vtx.size()) throw larbys("Invalid plane ID requested!");
      auto& plane = vtx[plane_id];
      if(cluster_id >= plane.size()) throw larbys("Invalid cluster ID requested!");
      auto& cluster = plane[cluster_id];
      if(atom_id >= cluster.size()) throw larbys("Invalid atomic ID requested!");
      return cluster[atom_id];
    }

    std::vector<size_t> PCACandidatesData::index_cluster(size_t cluster_id, size_t plane_id, size_t vtx_id) const
    {
      std::vector<size_t> res;
      if(vtx_id == kINVALID_SIZE) vtx_id = 0;
      if(vtx_id >= _index_map.size()) throw larbys("Invalid vertex ID requested!");
      auto& vtx = _index_map[vtx_id];
      if(plane_id >= vtx.size()) throw larbys("Invalid plane ID requested!");
      auto& plane = vtx[plane_id];
      if(cluster_id >= plane.size()) throw larbys("Invalid cluster ID requested!");
      auto& cluster = plane[cluster_id];
      res.reserve(cluster.size());
      for(auto const& atom_id : cluster) {
	if(atom_id!=kINVALID_SIZE)
	  res.push_back(atom_id);
      }
      return res;
    }

    std::vector<size_t> PCACandidatesData::index_plane(size_t plane_id, size_t vtx_id) const
    {
      std::vector<size_t> res;
      if(vtx_id == kINVALID_SIZE) vtx_id = 0;
      if(vtx_id >= _index_map.size()) throw larbys("Invalid vertex ID requested!");
      auto& vtx = _index_map[vtx_id];
      if(plane_id >= vtx.size()) throw larbys("Invalid plane ID requested!");
      auto& plane = vtx[plane_id];
      for(auto const& cluster : plane) {
	res.reserve(res.size() + cluster.size());
	for(auto const& atom : cluster)
	  if(atom!=kINVALID_SIZE) res.push_back(atom);
      }
      return res;
    }

    const std::vector<geo2d::Line<float> >& PCACandidatesData::lines() const
    { return _line_v; }

    const geo2d::Line<float>& PCACandidatesData::line(size_t idx) const
    {
      if(idx >= _line_v.size()) throw larbys("Invalid line ID requested!");
      return _line_v[idx];
    }

    void PCACandidatesData::insert_points(const std::vector<geo2d::Vector<float> >& pts,
					  size_t plane_id, size_t vtx_id)
    {
      if(plane_id >=3) throw larbys("Invalid plane_id requested!");
      if(vtx_id == kINVALID_SIZE) vtx_id = 0;
      if(_points_vv.size() <= vtx_id) {
	_points_vv.resize(vtx_id+1);
	for(auto& points_v : _points_vv)
	  points_v.resize(3);
      }
      _points_vv[vtx_id][plane_id] = pts;
    }

    void PCACandidatesData::emplace_points(std::vector<geo2d::Vector<float> >&& pts,
					   size_t plane_id, size_t vtx_id)
    {
      if(plane_id >=3) throw larbys("Invalid plane_id requested!");
      if(vtx_id == kINVALID_SIZE) vtx_id = 0;
      if(_points_vv.size() <= vtx_id) {
	_points_vv.resize(vtx_id+1);
	for(auto& points_v : _points_vv)
	  points_v.resize(3);
      }
      _points_vv[vtx_id][plane_id] = std::move(pts);
    }

    const std::vector<geo2d::Vector<float> >& PCACandidatesData::points(size_t plane_id, size_t vtx_id) const
    {
      if(vtx_id == kINVALID_SIZE) vtx_id = 0;
      if(_points_vv.size() <= vtx_id) throw larbys("Invalid vertex id requested...");
      auto const& points_v = _points_vv[vtx_id];
      if(points_v.size() <= plane_id) throw larbys("Invalid plane id requested...");
      return points_v[plane_id];
    }
  }
}
#endif
