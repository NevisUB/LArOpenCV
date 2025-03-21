#ifndef __ALGODATA_REFINE2DVERTEXDATA_CXX__
#define __ALGODATA_REFINE2DVERTEXDATA_CXX__

#include "Refine2DVertexData.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {

  namespace data {
    
    //////////////////////////////////////////////////////////////
    void Refine2DVertexData::Clear() {

      _num_planes = 3;      
      _type_v.clear();
      _vtx3d_v.clear();
      _circle_vtx_vv.clear();

    }

    size_t Refine2DVertexData::num_vertex() const
    { return _vtx3d_v.size(); }
    
    const std::vector<larocv::data::Vertex3D>& Refine2DVertexData::get_vertex() const
    { return _vtx3d_v; }
    
    const larocv::data::Vertex3D& Refine2DVertexData::get_vertex(size_t vtx_id) const
    {
      if(vtx_id >= _vtx3d_v.size()) throw larbys("Invalid vertex id requested!");
      return _vtx3d_v[vtx_id];
    }

    const std::vector<unsigned short>& Refine2DVertexData::get_type() const
    { return _type_v; }
    
    unsigned short Refine2DVertexData::get_type(size_t vtx_id) const
    {
      if(vtx_id >= _vtx3d_v.size()) throw larbys("Invalid vertex id requested!");
      return _type_v[vtx_id];
    }
    
    const std::vector<std::vector<larocv::data::CircleVertex> >& Refine2DVertexData::get_circle_vertex() const
    { return _circle_vtx_vv; }
    
    const std::vector<larocv::data::CircleVertex>& Refine2DVertexData::get_circle_vertex(size_t vtx_id) const
    {
      if(vtx_id >= _vtx3d_v.size()) throw larbys("Invalid vertex id requested!");
      return _circle_vtx_vv[vtx_id];
    }
    
    const larocv::data::CircleVertex& Refine2DVertexData::get_circle_vertex(size_t vtx_id, size_t plane) const
    {
      if(plane >= _num_planes) throw larbys("Invalid plane id requested!");
      if(vtx_id >= _vtx3d_v.size()) throw larbys("Invalid vertex id requested!");
      return _circle_vtx_vv[vtx_id][plane];
    }
    
    void Refine2DVertexData::push_back(unsigned short type,
				       const larocv::data::Vertex3D& vtx,
				       const std::vector<larocv::data::CircleVertex>& circle_v)
    {
      if(circle_v.size() != _num_planes) throw larbys("# CircleVertex != # planes!");
      _type_v.push_back(type);
      _circle_vtx_vv.push_back(circle_v);
      _vtx3d_v.push_back(vtx);
      _vtx3d_v.back()._id = _vtx3d_v.size() - 1;
    }
    
    void Refine2DVertexData::emplace_back(unsigned short type,
					  larocv::data::Vertex3D&& vtx,
					  std::vector<larocv::data::CircleVertex>&& circle_v)
    {
      if(circle_v.size() != _num_planes) throw larbys("# CircleVertex != # planes!");
      _type_v.push_back(type);
      _circle_vtx_vv.emplace_back(std::move(circle_v));
      _vtx3d_v.emplace_back(std::move(vtx));
      _vtx3d_v.back()._id = _vtx3d_v.size() - 1;
    }
    
  }
}

#endif
