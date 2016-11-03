#ifndef __ALGODATA_VERTEXCLUSTERDATA_CXX__
#define __ALGODATA_VERTEXCLUSTERDATA_CXX__

#include "Core/larbys.h"
#include "VertexClusterData.h"

namespace larocv {
  
  size_t VertexCluster::num_planes() const
  { return _cluster_vv.size(); }

  const larocv::Vertex3D& VertexCluster::get_vertex() const
  { return _vtx; }

  const larocv::CircleVertex& VertexCluster::get_circle_vertex(size_t plane) const
  {
    if(plane >= _circle_vtx_v.size()) throw larbys("Invalid plane requested!");
    return _circle_vtx_v[plane];
  }

  const std::vector< std::vector<geo2d::Vector<float> > >& VertexCluster::get_clusters(size_t plane) const
  {
    if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
    return _cluster_vv[plane];
  }

  void VertexCluster::clear()
  {
    _vtx = Vertex3D();
    _vtx.vtx2d_v.resize(_circle_vtx_v.size());
    for( auto& v : _circle_vtx_v ) v = CircleVertex();
    for( auto& v : _cluster_vv   ) v.clear();
  }

  void VertexCluster::num_planes(size_t n)
  {
    _circle_vtx_v.resize(n);
    _cluster_vv.resize(n);
    clear();
  }

  void VertexCluster::set_vertex(const larocv::Vertex3D& vtx)
  {
    if(_circle_vtx_v.empty()) num_planes(vtx.vtx2d_v.size());
    _vtx = vtx;
  }
  
  void VertexCluster::set_vertex(const larocv::Vertex3D& vtx,
				 const std::vector<larocv::CircleVertex>& circle_vtx_v)
  {
    if(vtx.vtx2d_v.size() != circle_vtx_v.size())
      throw larbys("2D plane count mismatch!");

    num_planes(vtx.vtx2d_v.size());
    _vtx = vtx;
    _circle_vtx_v = circle_vtx_v;
  }

  void VertexCluster::append(size_t plane, const std::vector<geo2d::Vector<float> >& cluster)
  {
    if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
    _cluster_vv[plane].push_back(cluster);
  }

  void VertexCluster::emplace(size_t plane, std::vector<geo2d::Vector<float> >&& cluster)
  {
    if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
    _cluster_vv[plane].emplace_back(std::move(cluster));
  }

}

#endif
