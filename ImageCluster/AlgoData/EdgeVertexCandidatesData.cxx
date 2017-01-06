#ifndef __ALGODATA_EDGEVERTEXCANDIDATESDATA_CXX__
#define __ALGODATA_EDGEVERTEXCANDIDATESDATA_CXX__

#include "EdgeVertexCandidatesData.h"
#include "Core/larbys.h"

namespace larocv {

  namespace data {

    //////////////////////////////////////////////////////////////
    
    void EdgeVertexCandidatesData::Clear()
    {
      _edge_vtx_v.clear();
      return;
    }

    void EdgeVertexCandidatesData::insert(size_t vtx_id, EdgeVertexCandidate& edge_vtx)
    {
      while(_edge_vtx_v.size() <= vtx_id) {
	_edge_vtx_v.push_back(EdgeVertexCandidate());
	_edge_vtx_v.back()._id = _edge_vtx_v.size() - 1;
      }
      _edge_vtx_v[vtx_id] = edge_vtx;
      _edge_vtx_v[vtx_id]._id = vtx_id;
    }
    
    void EdgeVertexCandidatesData::move(size_t vtx_id, EdgeVertexCandidate&& edge_vtx)
    {
      //thanks kazu
      while(_edge_vtx_v.size() <= vtx_id) {
	_edge_vtx_v.push_back(EdgeVertexCandidate());
	_edge_vtx_v.back()._id = _edge_vtx_v.size() - 1;
      }
      _edge_vtx_v[vtx_id] = std::move(edge_vtx);
      _edge_vtx_v[vtx_id]._id = vtx_id;
    }
    
    const EdgeVertexCandidate& EdgeVertexCandidatesData::get_edge_candidate(size_t vtx_id) const
    {
      if (vtx_id >= _edge_vtx_v.size()) throw larbys("Invalid edge candidate requested");
      return _edge_vtx_v[vtx_id];
    }
    
    const std::vector<EdgeVertexCandidate>& EdgeVertexCandidatesData::get_edge_candidates() const
    {
      return _edge_vtx_v;
    }
    
  }
}
#endif
