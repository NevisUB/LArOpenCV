#ifndef __ALGODATA_DQDXPROFILERDATA_CXX__
#define __ALGODATA_DQDXPROFILERDATA_CXX__

#include "dQdXProfilerData.h"
#include "Core/larbys.h"

namespace larocv {

  namespace data {

    //////////////////////////////////////////////////////////////

    size_t ParticledQdX::id() const
    { return _cluster_id; }
    
    void ParticledQdX::push_back(const std::vector<float>& dqdx)
    {
      //std::vector<float>  _dqdx_v;
      //std::vector<size_t> _defects_v;
    }
    
    void ParticledQdX::emplace_back(const std::vector<float>& dqdx)
    {

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
    
    void ParticledQdXArray::push_back(size_t plane, const larocv::data::ParticledQdX& cluster)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      cluster_v.push_back(cluster);
      cluster_v.back()._cluster_id = cluster_v.size() - 1;
    }

    void ParticledQdXArray::emplace_back(size_t plane, larocv::data::ParticledQdX&& cluster)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      cluster_v.emplace_back(std::move(cluster));
      cluster_v.back()._cluster_id = cluster_v.size() - 1;
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
