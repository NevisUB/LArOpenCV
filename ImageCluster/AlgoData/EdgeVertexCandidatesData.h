#ifndef __ALGODATA_EDGEVERTEXCANDIDATESDATA_H__
#define __ALGODATA_EDGEVERTEXCANDIDATESDATA_H__

#include "Core/Line.h"
#include "Core/LArOCVTypes.h"
#include "Base/AlgoDataBase.h"
#include "AlgoDataVertex.h"
#include "Core/VectorArray.h"
#include "Core/Circle.h"

namespace larocv {

  namespace data {
    
    class EdgeVertexCandidatesData;

    //Vertex3D _edge_vtx;
    //Vertex3D _side_vtx;
    
    class EdgeVertexCandidate {
    public:
      EdgeVertexCandidate() :
	_id       (kINVALID_SIZE) ,
	max_index (kINVALID_SIZE),
	max_value (kINVALID_FLOAT),
	ledge     (false),
	redge     (false),
	ridx      (kINVALID_SIZE),
	lidx      (kINVALID_SIZE),
	sidx      (kINVALID_SIZE),
	eidx      (kINVALID_SIZE),
	edge_pt   (kINVALID_FLOAT,kINVALID_FLOAT),
	base_rmean (kINVALID_FLOAT),
	base_rsigma(kINVALID_FLOAT)
      { clear(); }
	
      ~EdgeVertexCandidate() {}
      
      void clear() {
	rmean_dqdx_v.clear();
	rsigma_dqdx_v.clear();
	side_pt_v.clear();
      }
      
      const size_t id() const { return _id; }
      
      std::vector<float> rmean_dqdx_v;
      std::vector<float> rsigma_dqdx_v;

      size_t max_index;
      float  max_value;
      
      bool ledge;
      bool redge;

      size_t ridx;
      size_t lidx;
      size_t eidx;
      size_t sidx;

      float base_rmean;
      float base_rsigma;

      geo2d::Vector<float> edge_pt;
      geo2d::VectorArray<float> side_pt_v;

      geo2d::Circle<float> circle;
      
    private:
      size_t _id;
      
    };


    class EdgeVertexCandidateArray {
      friend class EdgeVertexCandidatesData;
      
    public:
      EdgeVertexCandidateArray() { clear(); }
      ~EdgeVertexCandidateArray() {}

      void clear() {
	_edge_vtx_candidate_v.clear();
	_edge_vtx_candidate_v.resize(3);
      }
      void insert(short plane_id,EdgeVertexCandidate edge_vtx_candidate);
      const EdgeVertexCandidate& get_edge_vertex_candidate(short plane_id) const;
      const size_t id() const { return _id; }
    private:
      std::vector<EdgeVertexCandidate> _edge_vtx_candidate_v;
      size_t _id;
    };
    
    /**
       \class EdgeVertexCandidatesData
       @brief only for vic
    */
    
    class EdgeVertexCandidatesData : public AlgoDataBase {
      
    public:
    
      /// Default constructor
      EdgeVertexCandidatesData()
      {  Clear(); }    
      /// Default destructor
      ~EdgeVertexCandidatesData(){}
      
      void Clear();

      void insert(size_t vtx_id, EdgeVertexCandidateArray& edge_vtx_arr);
      
      void move(size_t vtx_id, EdgeVertexCandidateArray&& edge_vtx_arr);
      
      const EdgeVertexCandidateArray& get_edge_candidate(size_t vtx_id) const;
      
      const std::vector<EdgeVertexCandidateArray>& get_edge_candidates() const;
      
    private:

      std::vector<EdgeVertexCandidateArray> _edge_vtx_arr_v;
      
    };
  }  
}
#endif
/** @} */ // end of doxygen group
