#ifndef __ALGODATA_EDGEVERTEXCANDIDATESDATA_H__
#define __ALGODATA_EDGEVERTEXCANDIDATESDATA_H__

#include "Core/Line.h"
#include "Core/LArOCVTypes.h"
#include "Base/AlgoDataBase.h"
#include "AlgoDataVertex.h"

namespace larocv {

  namespace data {
    
    class EdgeVertexCandidatesData;

    //Vertex3D _edge_vtx;
    //Vertex3D _side_vtx;
    
    class EdgeVertexCandidate {
      friend class EdgeVertexCandidatesData;
    public:
      EdgeVertexCandidate() :
	_id       (kINVALID_SIZE) ,
	max_index (kINVALID_SIZE),
	max_value (kINVALID_FLOAT),
	ledge     (false),
	redge     (false),
	ridx      (kINVALID_SIZE),
	lidx      (kINVALID_SIZE),
	edge_pt   (kINVALID_FLOAT,kINVALID_FLOAT)
      { clear(); }
	
      ~EdgeVertexCandidate() {}
      
      void clear() {
	rmean_dqdx.clear();
	rsigma_dqdx.clear();
      }
      
      const size_t id() const { return _id; }
      
      std::vector<float> rmean_dqdx;
      std::vector<float> rsigma_dqdx;

      size_t max_index;
      float  max_value;
      
      bool ledge;
      bool redge;

      size_t ridx;
      size_t lidx;

      geo2d::Vector<float> edge_pt;
      geo2d::Vector<float> side_pt;
      
    private:
      size_t _id;
      
    };
    
    /**
       \class EdgeVertexCandidatesData
       @brief only for vic
    */
    
    class EdgeVertexCandidatesData : public AlgoDataBase {
      
    public:
    
      /// Default constructor
      EdgeVertexCandidatesData(std::string name="NoName", AlgorithmID_t id=0)
	: AlgoDataBase(name,id)
      {  Clear(); }    
      /// Default destructor
      ~EdgeVertexCandidatesData(){}
      
      void Clear();

      void insert(size_t vtx_id, EdgeVertexCandidate& edge_vtx);
      
      void move(size_t vtx_id, EdgeVertexCandidate&& edge_vtx);
      
      const EdgeVertexCandidate& get_edge_candidate(size_t vtx_id) const;
      
      const std::vector<EdgeVertexCandidate>& get_edge_candidates() const;
      
    private:

      std::vector<EdgeVertexCandidate> _edge_vtx_v;
      
    };
  }  
}
#endif
/** @} */ // end of doxygen group
