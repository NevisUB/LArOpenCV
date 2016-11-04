/**
 * \file PCACandidatesData.h
 *
 * \ingroup AlgoData
 * 
 * \brief Class def header for a class PCACandidatesData
 *
 * @author vic
 */

/** \addtogroup AlgoData

    @{*/

#ifndef __ALGODATA_PCACANDIDATESDATA_H__
#define __ALGODATA_PCACANDIDATESDATA_H__

#include "Core/Line.h"
#include "Core/LArOCVTypes.h"
#include "Base/AlgoDataBase.h"
namespace larocv {

  namespace data {

    class VertexPlaneClusterAtomicMap {
    public:
      VertexPlaneClusterAtomicMap()
	: vertex_id  (kINVALID_SIZE)
	, plane_id   (kINVALID_SIZE)
	, cluster_id (kINVALID_SIZE)
	, atomic_id  (kINVALID_SIZE)
      {}
      ~VertexPlaneClusterAtomicMap() {}
      size_t vertex_id;
      size_t plane_id;
      size_t cluster_id;
      size_t atomic_id;
    };
    
    /**
       \class PCACandidatesData
       @brief only for vic
    */
    class PCACandidatesData : public AlgoDataBase {
      
    public:
    
      /// Default constructor
      PCACandidatesData(std::string name="NoName", AlgorithmID_t id=0)
	: AlgoDataBase(name,id)
      {  Clear(); }    
      /// Default destructor
      ~PCACandidatesData(){}
      /// Clear method override
      void Clear();

      AlgorithmID_t _input_id;

      /// register a pca & point for an atomic cluster by copy
      void insert(const geo2d::Line<float>& line,
		  size_t atom_id, size_t cluster_id, size_t plane, size_t vtx_id=kINVALID_SIZE);
      /// register a pca & point for an atomic cluster by move
      void move(geo2d::Line<float>&& line,
		size_t atom_id, size_t cluster_id, size_t plane, size_t vtx_id=kINVALID_SIZE);
      /// register points crossing by pca-to-pca by copy
      void insert_points(const std::vector<geo2d::Vector<float> >& pts, size_t plane_id, size_t vtx_id=kINVALID_SIZE);
      /// register points crossing by pca-to-pca by move
      void emplace_points(std::vector<geo2d::Vector<float> >&& pts, size_t plane_id, size_t vtx_id=kINVALID_SIZE);
      /// query an index of PCA/Points via atomic/cluster/plane and optionally vertex id
      size_t index_atom(size_t atom_id, size_t cluster_id, size_t plane, size_t vtx_id=kINVALID_SIZE) const;
      /// query a set of indecies of PCA/Points via cluster/plane and optionally vertex id
      std::vector<size_t> index_cluster(size_t cluster_id, size_t plane, size_t vtx_id=kINVALID_SIZE) const;
      /// query a set of indecies of PCA/Points via plane and optionally vertex id
      std::vector<size_t> index_plane(size_t plane, size_t vtx_id=kINVALID_SIZE) const;
      /// query a set of IDs (vertex/plane/cluster/atomic) from a pca line/points index
      const VertexPlaneClusterAtomicMap& parent_id(size_t idx) const;
      /// PCA line array access
      const std::vector<geo2d::Line<float> >& lines() const;
      /// PCA line access
      const geo2d::Line<float>& line(size_t idx) const;
      /// PCA points access
      const std::vector<geo2d::Vector<float> >& points(size_t plane_id, size_t vtx_id=kINVALID_SIZE) const;
      
    private:
      /// method to register book keeping of IDs
      void insert_id(size_t atom_id, size_t cluster_id, size_t plane_id, size_t vtx_id, size_t pca_id);      
      /// PCA Line index mapper ... [vtx][plane][cluster][atomic] returns you an index of stored line/point
      std::vector< std::vector< std::vector< std::vector< size_t > > > > _index_map;
      /// PCA Line index mapper ... pca index returns associated object information
      std::vector<larocv::data::VertexPlaneClusterAtomicMap> _reverse_index_map;
      /// PCA line per atomic
      std::vector<geo2d::Line<float> >  _line_v;
      /// Points associated with PCA line per atomic-atomic pair
      std::vector< std::vector< std::vector<geo2d::Vector<float> > > > _points_vv;
    };
  }  
}
#endif
/** @} */ // end of doxygen group
