#ifndef __ALGODATA_VERTEXCLUSTERDATA_CXX__
#define __ALGODATA_VERTEXCLUSTERDATA_CXX__

#include "Core/larbys.h"
#include "VertexClusterData.h"

namespace larocv {

  namespace data {

    void ParticleCluster::clear()
    { _ctor.clear(); }
    
    size_t ParticleCluster::id() const
    { return _cluster_id; }
    
    ////////////////////////////////////////////////////////////////////
    
    size_t ParticleClusterArray::id() const
    { return _vtx.id(); }
    
    size_t ParticleClusterArray::num_planes() const
    { return _cluster_vv.size(); }
    
    size_t ParticleClusterArray::num_clusters() const
    { return _cluster_vv.front().size(); }
    
    const larocv::data::Vertex3D& ParticleClusterArray::get_vertex() const
    { return _vtx; }
    
    const larocv::data::CircleVertex& ParticleClusterArray::get_circle_vertex(size_t plane) const
    {
      if(plane >= _circle_vtx_v.size()) throw larbys("Invalid plane requested!");
      return _circle_vtx_v[plane];
    }
    
    const std::vector<larocv::data::ParticleCluster>& ParticleClusterArray::get_clusters(size_t plane) const
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      return _cluster_vv[plane];
    }
    
    void ParticleClusterArray::clear()
    {
      _vtx.clear();
      _circle_vtx_v.resize(3);
      _vtx.vtx2d_v.resize(3);
      _cluster_vv.resize(3);
      for( auto& v : _circle_vtx_v ) v = CircleVertex();
      for( auto& v : _cluster_vv   ) v.clear();
    }
    
    void ParticleClusterArray::num_planes(size_t n)
    {
      _circle_vtx_v.resize(n);
      _cluster_vv.resize(n);
      clear();
    }
    
    void ParticleClusterArray::set_vertex(const larocv::data::Vertex3D& vtx)
    {
      if(_circle_vtx_v.empty()) num_planes(vtx.vtx2d_v.size());
      _vtx = vtx;
    }
    
    void ParticleClusterArray::set_vertex(const larocv::data::Vertex3D& vtx,
					  const std::vector<larocv::data::CircleVertex>& circle_vtx_v)
    {
      if(vtx.vtx2d_v.size() != circle_vtx_v.size())
	throw larbys("2D plane count mismatch!");
      
      num_planes(vtx.vtx2d_v.size());
      _vtx = vtx;
      _circle_vtx_v = circle_vtx_v;
    }
    
    void ParticleClusterArray::insert(size_t plane, const larocv::data::ParticleCluster& cluster)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      if(cluster.id() == kINVALID_SIZE) throw larbys("Cannot insert cluster with an invalid id");
      auto& cluster_v = _cluster_vv[plane];
      if(cluster_v.size() <= cluster.id()) cluster_v.resize(cluster.id()+1);
      cluster_v[cluster.id()] = cluster;
    }
    
    void ParticleClusterArray::move(size_t plane, larocv::data::ParticleCluster&& cluster)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      if(cluster.id() == kINVALID_SIZE) throw larbys("Cannot insert cluster with an invalid id");
      auto& cluster_v = _cluster_vv[plane];
      if(cluster_v.size() <= cluster.id()) cluster_v.resize(cluster.id()+1);
      cluster_v[cluster.id()] = std::move(cluster);
    }
  }
}

#endif
