#ifndef __ALGODATA_REFINE2DVERTEXDATA_CXX__
#define __ALGODATA_REFINE2DVERTEXDATA_CXX__

#include "Refine2DVertexData.h"
#include "Core/larbys.h"

namespace larocv {

  namespace data {
    
    //////////////////////////////////////////////////////////////
    
    void CircleVertex::Clear()
    {
      center.x = center.y = -1;
      error.x  = error.y  = -1;
      radius   = -1;
      xs_v.clear();
      dtheta_v.clear();
    }
    
    float CircleVertex::sum_dtheta() const
    { float res=0; for(auto const& d : dtheta_v) res += d; return res; }
    
    void Refine2DVertexPlaneData::Clear()
    {
      _valid_plane = false;
      _init_vtx_v.clear();
      _scan_rect_v.clear();
      _circle_scan_v.clear();
    }
    
    //////////////////////////////////////////////////////////////
    void Refine2DVertexData::Clear() {
      
      _plane_data.resize(3);
      for(auto& d : _plane_data) d.Clear();

      _type_v.clear();
      _vtx3d_v.clear();
      _circle_vtx_vv.clear();
      
      _cand_valid_v.clear();
      _cand_score_v.clear();
      _cand_vtx_v.clear();
      
      _cand_valid_v.resize(3,false);
      _cand_score_v.resize(3,-1);
      _cand_vtx_v.resize(3);
      
      _cand_xs_vv.resize(3);
      for(auto& d : _cand_xs_vv)      d.clear();    
    }

    size_t Refine2DVertexData::num_vertex() const
    { return _vtx3d_v.size(); }
    
    const std::vector<larocv::data::Refine2DVertexPlaneData>& Refine2DVertexData::get_plane_data() const
    { return _plane_data; }
    
    const larocv::data::Refine2DVertexPlaneData& Refine2DVertexData::get_plane_data(size_t plane) const
    {
      if(plane >= _plane_data.size()) throw larbys("Invalid plane id requested!");
      return _plane_data[plane];
    }
    
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
      if(plane >= _plane_data.size()) throw larbys("Invalid plane id requested!");
      if(vtx_id >= _vtx3d_v.size()) throw larbys("Invalid vertex id requested!");
      return _circle_vtx_vv[vtx_id][plane];
    }
    
    larocv::data::Refine2DVertexPlaneData& Refine2DVertexData::get_plane_data_writeable(size_t plane)
    {
      if(plane >= _plane_data.size()) throw larbys("Invalid plane id requested!");
      return _plane_data[plane];
    }
    
    void Refine2DVertexData::push_back(unsigned short type,
				       const larocv::data::Vertex3D& vtx,
				       const std::vector<larocv::data::CircleVertex>& circle_v)
    {
      if(circle_v.size() != _plane_data.size()) throw larbys("# CircleVertex != # planes!");
      _type_v.push_back(type);
      _circle_vtx_vv.push_back(circle_v);
      _vtx3d_v.push_back(vtx);
      _vtx3d_v.back()._id = _vtx3d_v.size() - 1;
    }
    
    void Refine2DVertexData::emplace_back(unsigned short type,
					  larocv::data::Vertex3D&& vtx,
					  std::vector<larocv::data::CircleVertex>&& circle_v)
    {
      if(circle_v.size() != _plane_data.size()) throw larbys("# CircleVertex != # planes!");
      _type_v.push_back(type);
      _circle_vtx_vv.emplace_back(std::move(circle_v));
      _vtx3d_v.emplace_back(std::move(vtx));
      _vtx3d_v.back()._id = _vtx3d_v.size() - 1;
    }
    
  }
}

#endif
