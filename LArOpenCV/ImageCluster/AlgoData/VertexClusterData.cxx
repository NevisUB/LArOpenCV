#ifndef __ALGODATA_VERTEXCLUSTERDATA_CXX__
#define __ALGODATA_VERTEXCLUSTERDATA_CXX__

#include "LArOpenCV/Core/larbys.h"
#include "VertexClusterData.h"

namespace larocv {

  namespace data {

    void ParticleCluster::clear()
    { _ctor.clear(); _num_pixel = 0;}
    
    size_t ParticleCluster::id() const
    { return _cluster_id; }
    
    ////////////////////////////////////////////////////////////////////
    
    size_t ParticleClusterArray::id() const
    { return _vtx.id(); }
    
    size_t ParticleClusterArray::num_planes() const
    { return _cluster_vv.size(); }
    
    size_t ParticleClusterArray::num_clusters(size_t plane) const
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      return _cluster_vv[plane].size();
    }

    size_t ParticleClusterArray::num_pixels(size_t plane) const
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      return _num_pixel_v[plane];
    }

    float ParticleClusterArray::num_pixel_fraction(size_t plane) const
    {
      float res = 0.;
      for(auto const& c : get_clusters(plane))
	res += c._num_pixel;

      if(_num_pixel_v[plane]<1) return 0.;
      else return res / ((float)(_num_pixel_v[plane]));
    }
    
    float ParticleClusterArray::num_pixel_fraction(size_t plane, size_t cluster_id) const
    {
      auto const& clusters = get_clusters(plane);

      if(_num_pixel_v[plane]<1) return 0.;
      
      if(cluster_id >= clusters.size()) throw larbys("Invalid cluster id requested!");
      auto const& c = clusters[cluster_id];

      return (float)(c._num_pixel) / (float)(_num_pixel_v[plane]);
    }
    
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
      _num_pixel_v.resize(3);
      for( auto& v : _num_pixel_v  ) v = 0;
      for( auto& v : _circle_vtx_v ) v = CircleVertex();
      for( auto& v : _cluster_vv   ) v.clear();
    }
    
    void ParticleClusterArray::set_vertex(const larocv::data::Vertex3D& vtx)
    {
      if(_circle_vtx_v.size() != vtx.vtx2d_v.size()) throw larbys("Invalid # planes!");
      _vtx = vtx;
    }
    
    void ParticleClusterArray::set_vertex(const larocv::data::Vertex3D& vtx,
					  const std::vector<larocv::data::CircleVertex>& circle_vtx_v)
    {
      if(vtx.vtx2d_v.size() != circle_vtx_v.size())
	throw larbys("2D plane count mismatch!");
      if(_circle_vtx_v.size() != vtx.vtx2d_v.size()) throw larbys("Invalid # planes!");
      _vtx = vtx;
      _circle_vtx_v = circle_vtx_v;
    }

    void ParticleClusterArray::set_num_pixel(size_t plane, size_t count)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane reuqested!");
      _num_pixel_v[plane] = count;
    }

    ParticleCluster& ParticleClusterArray::make_cluster(size_t plane)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      cluster_v.push_back(ParticleCluster());
      cluster_v.back()._cluster_id = cluster_v.size() - 1;
      return cluster_v.back();
    }
    
    void ParticleClusterArray::push_back(size_t plane, const larocv::data::ParticleCluster& cluster)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      cluster_v.push_back(cluster);
      cluster_v.back()._cluster_id = cluster_v.size() - 1;
    }
    
    void ParticleClusterArray::emplace_back(size_t plane, larocv::data::ParticleCluster&& cluster)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      cluster_v.emplace_back(cluster);
      cluster_v.back()._cluster_id = cluster_v.size() - 1;
    }
  }
}

#endif
