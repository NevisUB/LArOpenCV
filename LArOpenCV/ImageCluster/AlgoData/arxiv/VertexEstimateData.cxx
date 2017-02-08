#ifndef __ALGODATA_VERTEXESTIMATEDATA_CXX__
#define __ALGODATA_VERTEXESTIMATEDATA_CXX__

#include "VertexEstimateData.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {

  namespace data {

    //////////////////////////////////////////////////////////////
    void VertexEstimateData::Clear() {
      _num_planes = 3;      
      _type_v.clear();
      _vtx3d_v.clear();
      _circle_vtx_vv.clear();
    }

    size_t VertexEstimateData::num_vertex() const
    { return _vtx3d_v.size(); }
    
    const std::vector<Vertex3D>& VertexEstimateData::get_vertex() const
    { return _vtx3d_v; }
    
    const Vertex3D& VertexEstimateData::get_vertex(size_t vtx_id) const
    {
      if(vtx_id >= _vtx3d_v.size()) throw larbys("Invalid vertex id requested!");
      return _vtx3d_v[vtx_id];
    }

    const std::vector<unsigned short>& VertexEstimateData::get_type() const
    { return _type_v; }
    
    unsigned short VertexEstimateData::get_type(size_t vtx_id) const
    {
      if(vtx_id >= _vtx3d_v.size()) throw larbys("Invalid vertex id requested!");
      return _type_v[vtx_id];
    }
    
    const std::vector<std::vector<CircleVertex> >& VertexEstimateData::get_circle_vertex() const
    { return _circle_vtx_vv; }
    
    const std::vector<CircleVertex>& VertexEstimateData::get_circle_vertex(size_t vtx_id) const
    {
      if(vtx_id >= _vtx3d_v.size()) throw larbys("Invalid vertex id requested!");
      return _circle_vtx_vv[vtx_id];
    }
    
    const CircleVertex& VertexEstimateData::get_circle_vertex(size_t vtx_id, size_t plane) const
    {
      if(plane >= _num_planes) throw larbys("Invalid plane id requested!");
      if(vtx_id >= _vtx3d_v.size()) throw larbys("Invalid vertex id requested!");
      return _circle_vtx_vv[vtx_id][plane];
    }


    void VertexEstimateData::push_back(unsigned short type,
				       const Vertex3D& vtx)
				       
    {
      if (type < 2) throw larbys("Setting type less than 2 requires circle vertex estimation");
      _type_v.push_back(type);
      _vtx3d_v.push_back(vtx);
      _vtx3d_v.back()._id = _vtx3d_v.size() - 1;
    }
    
    void VertexEstimateData::emplace_back(unsigned short type,
					  Vertex3D&& vtx)
					  
    {
      if (type < 2) throw larbys("Setting type less than 2 requires circle vertex estimation");
      _type_v.push_back(type);
      _vtx3d_v.emplace_back(std::move(vtx));
      _vtx3d_v.back()._id = _vtx3d_v.size() - 1;
    }

    
    void VertexEstimateData::push_back(unsigned short type,
				       const Vertex3D& vtx,
				       const std::vector<CircleVertex>& circle_v)
    {
      if(circle_v.size() != _num_planes) throw larbys("# CircleVertex != # planes!");
      _type_v.push_back(type);
      _circle_vtx_vv.push_back(circle_v);
      _vtx3d_v.push_back(vtx);
      _vtx3d_v.back()._id = _vtx3d_v.size() - 1;
    }
    
    void VertexEstimateData::emplace_back(unsigned short type,
					  Vertex3D&& vtx,
					  std::vector<CircleVertex>&& circle_v)
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
