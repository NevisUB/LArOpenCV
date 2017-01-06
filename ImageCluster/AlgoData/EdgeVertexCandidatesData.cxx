#ifndef __ALGODATA_EDGEVERTEXCANDIDATESDATA_CXX__
#define __ALGODATA_EDGEVERTEXCANDIDATESDATA_CXX__

#include "EdgeVertexCandidatesData.h"
#include "Core/larbys.h"

namespace larocv {

  namespace data {

    //////////////////////////////////////////////////////////////
    
    void EdgeVertexCandidateArray::insert(short plane_id,EdgeVertexCandidate edge_vtx_candidate) {
      if (plane_id < 0 or plane_id > 3) throw larbys();
      _edge_vtx_candidate_v[plane_id] = edge_vtx_candidate;
    }
    
    const EdgeVertexCandidate& EdgeVertexCandidateArray::get_edge_vertex_candidate(short plane_id) const {
      if (plane_id < 0 or plane_id > 3) throw larbys();
      return _edge_vtx_candidate_v[plane_id];
    }
    
    //////////////////////////////////////////////////////////////
    
    void EdgeVertexCandidatesData::Clear()
    {
      _edge_vtx_arr_v.clear();
      return;
    }

    void EdgeVertexCandidatesData::insert(size_t vtx_id, EdgeVertexCandidateArray& edge_vtx_arr)
    {
      while(_edge_vtx_arr_v.size() <= vtx_id) {
	_edge_vtx_arr_v.push_back(EdgeVertexCandidateArray());
	_edge_vtx_arr_v.back()._id = _edge_vtx_arr_v.size() - 1;
      }
      _edge_vtx_arr_v[vtx_id] = edge_vtx_arr;
      _edge_vtx_arr_v[vtx_id]._id = vtx_id;
    }
    
    void EdgeVertexCandidatesData::move(size_t vtx_id, EdgeVertexCandidateArray&& edge_vtx_arr)
    {
      //thanks kazu
      while(_edge_vtx_arr_v.size() <= vtx_id) {
	_edge_vtx_arr_v.push_back(EdgeVertexCandidateArray());
	_edge_vtx_arr_v.back()._id = _edge_vtx_arr_v.size() - 1;
      }
      _edge_vtx_arr_v[vtx_id] = std::move(edge_vtx_arr);
      _edge_vtx_arr_v[vtx_id]._id = vtx_id;
    }
    
    const EdgeVertexCandidateArray& EdgeVertexCandidatesData::get_edge_candidate(size_t vtx_id) const
    {
      if (vtx_id >= _edge_vtx_arr_v.size()) throw larbys("Invalid edge candidate requested");
      return _edge_vtx_arr_v[vtx_id];
    }
    
    const std::vector<EdgeVertexCandidateArray>& EdgeVertexCandidatesData::get_edge_candidates() const
    {
      return _edge_vtx_arr_v;
    }
    
  }
}
#endif
