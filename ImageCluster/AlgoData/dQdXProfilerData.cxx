#ifndef __ALGODATA_DQDXPROFILERDATA_CXX__
#define __ALGODATA_DQDXPROFILERDATA_CXX__

#include "dQdXProfilerData.h"
#include "Core/larbys.h"

namespace larocv {

  namespace data {

    //////////////////////////////////////////////////////////////

    size_t ParticledQdX::id() const
    { return _cluster_id; }
    
    void ParticledQdX::push_back(size_t atom_id,
				 const geo2d::Vector<float>& start,
				 const std::vector<float>& dqdx)
    {
      _atom_id_v.push_back(atom_id);
      _start_pt_v.push_back(start);
      _start_index_v.push_back(_dqdx_v.size());
      _dqdx_v.reserve(_dqdx_v.size() + dqdx.size());
      for(auto const& v : dqdx) _dqdx_v.push_back(v);
    }

    const std::vector<float>& ParticledQdX::dqdx() const
    { return _dqdx_v; }

    size_t ParticledQdX::num_atoms() const
    { return _start_pt_v.size(); }

    const std::vector<size_t>& ParticledQdX::atom_start_index_array() const
    { return _start_index_v; }

    const std::vector<size_t>& ParticledQdX::atom_id_array() const
    { return _atom_id_v; }

    const std::vector<geo2d::Vector<float> >& ParticledQdX::atom_start_pt_array() const
    { return _start_pt_v; }

    size_t ParticledQdX::atom_start_index(size_t atom_index) const
    {
      if(atom_index >= _start_index_v.size()) throw larbys("Invalid atom index requested!");
      return _start_index_v[atom_index];
    }

    size_t ParticledQdX::atom_id(size_t atom_index) const
    {
      if(atom_index >= _atom_id_v.size()) throw larbys("Invalid atom index requested!");
      return _atom_id_v[atom_index];
    }
    
    const geo2d::Vector<float>& ParticledQdX::atom_start_pt(size_t atom_index) const
    {
      if(atom_index >= _start_pt_v.size()) throw larbys("Invalid atom index requested!");
      return _start_pt_v[atom_index];
    }

    //////////////////////////////////////////////////////////////

    size_t ParticledQdXArray::id() const
    { return _id; }

    size_t ParticledQdXArray::num_planes() const
    { return _cluster_vv.size(); }

    size_t ParticledQdXArray::num_clusters(size_t plane) const
    {
      return get_cluster(plane).size();
    }

    const std::vector<larocv::data::ParticledQdX>& ParticledQdXArray::get_cluster(size_t plane) const
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      return _cluster_vv[plane];
    }

    void ParticledQdXArray::clear()
    {
      _cluster_vv.clear();
      _cluster_vv.resize(3);
    }
    
    void ParticledQdXArray::insert(size_t plane, size_t cluster_id, const larocv::data::ParticledQdX& cluster)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      while(cluster_v.size() <= cluster_id) {
	cluster_v.push_back(ParticledQdX());
	cluster_v.back()._cluster_id = cluster_v.size() - 1;
      }
      cluster_v[cluster_id] = cluster;
      cluster_v[cluster_id]._cluster_id = cluster_id;
    }

    void ParticledQdXArray::move(size_t plane, size_t cluster_id, larocv::data::ParticledQdX&& cluster)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      while(cluster_v.size() <= cluster_id) {
	cluster_v.push_back(ParticledQdX());
	cluster_v.back()._cluster_id = cluster_v.size() - 1;
      }
      cluster_v[cluster_id] = std::move(cluster);
      cluster_v[cluster_id]._cluster_id = cluster_id;
    }

    larocv::data::ParticledQdX& ParticledQdXArray::make_cluster(size_t plane)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      cluster_v.push_back(ParticledQdX());
      cluster_v.back()._cluster_id = cluster_v.size() - 1;
      return cluster_v.back();
    }

    //////////////////////////////////////////////////////////////
    
    void dQdXProfilerData::Clear()
    { _vtx_cluster_v.clear(); }

    void dQdXProfilerData::insert(size_t vtx_id, const larocv::data::ParticledQdXArray& col)
    {
      while(_vtx_cluster_v.size() <= vtx_id) {
	_vtx_cluster_v.push_back(ParticledQdXArray());
	_vtx_cluster_v.back()._id = _vtx_cluster_v.size() - 1;
      }
      _vtx_cluster_v[vtx_id] = col;
      _vtx_cluster_v[vtx_id]._id = vtx_id;
    }
    
    void dQdXProfilerData::move(size_t vtx_id, larocv::data::ParticledQdXArray&& col)
    {
      while(_vtx_cluster_v.size() <= vtx_id) {
	_vtx_cluster_v.push_back(ParticledQdXArray());
	_vtx_cluster_v.back()._id = _vtx_cluster_v.size() - 1;
      }
      _vtx_cluster_v[vtx_id] = std::move(col);
      _vtx_cluster_v[vtx_id]._id = vtx_id;
    }

    const std::vector<larocv::data::ParticledQdXArray>& dQdXProfilerData::get_vertex_clusters() const
    { return _vtx_cluster_v; }

    const larocv::data::ParticledQdXArray& dQdXProfilerData::get_vertex_cluster(size_t vtx_id) const
    {
      if(vtx_id >= _vtx_cluster_v.size()) throw larbys("Invalid vertex id requested!");
      return _vtx_cluster_v[vtx_id];
    }
    
  }
}
#endif
