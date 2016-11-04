/**
 * \file dQdXProfilerData.h
 *
 * \ingroup AlgoData
 * 
 * \brief Class def header for a class dQdXProfiler
 *
 * @author kazuhiro
 */

/** \addtogroup AlgoData

    @{*/

#ifndef __ALGODATA_DQDXPROFILERDATA_H__
#define __ALGODATA_DQDXPROFILERDATA_H__

#include "Core/Line.h"
#include "Core/LArOCVTypes.h"
#include "Base/AlgoDataBase.h"
namespace larocv {

  namespace data {

    class ParticledQdXArray;
    class dQdXProfilerData;

    class ParticledQdX {
      friend class ParticledQdXArray;
    public:
      ParticledQdX() : _cluster_id(kINVALID_SIZE)
      {}
      ~ParticledQdX() {}
      /// particle cluster id
      size_t id() const;
      /// append dQ/dX from atomic contour
      void push_back(size_t atom_id,
		     const geo2d::Vector<float>& start,
		     const std::vector<float>& dqdx);
      /// queries # atoms in this dQ/dX
      size_t num_atoms() const;
      /// queries dq/dx index array that corresponds to atoms' start position
      const std::vector<size_t>& atom_start_index_array() const;
      /// queries atomic contours' id in the order added for dqdx construction
      const std::vector<size_t>& atom_id_array() const;
      /// queries 2D coordinate point array that corresponds atoms' start position
      const std::vector<geo2d::Vector<float> >& atom_start_pt_array() const;
      /// queries the dq/dx index that corresponds to an atom's start position
      size_t atom_start_index(size_t atom_index) const;
      /// queries atomic contour's id
      size_t atom_id(size_t atom_index) const;
      /// queries 2D coordinate point that corresponds to an atom's start position
      const geo2d::Vector<float>& atom_start_pt(size_t atom_index) const;

    private:
      /// unique ID (should respect Particle cluster id)
      size_t _cluster_id;
      /// resolution of dx
      double _dx_resolution;
      /// dQ/dX array
      std::vector<float>  _dqdx_v;
      /// atomic cluster's boundary (start) location in terms of dq/dx array index
      std::vector<size_t> _start_index_v;
      /// atomic cluster's boundary (start) location in 2D coordinate system
      std::vector<geo2d::Vector<float> > _start_pt_v;
      /// atomic cluster's id array
      std::vector<size_t> _atom_id_v;
    };

    class ParticledQdXArray {
      friend class dQdXProfilerData;
    public:
      ParticledQdXArray() {}
      ~ParticledQdXArray() {}

      /// get id
      size_t id() const;
      /// get # of planes
      size_t num_planes() const;
      /// get # clusters
      size_t num_clusters(size_t plane) const;
      /// plane-wise cluster list accessor
      const std::vector<larocv::data::ParticledQdX>& get_cluster(size_t plane_id) const;

      /// clear data attributes
      void clear();
      /// generate a new cluster
      ParticledQdX& make_cluster(size_t plane);
      /// Cluster (per plane per cluster) appender
      void insert(size_t plane, size_t cluster_id, const larocv::data::ParticledQdX& cluster);
      /// Cluster (per plane) appender
      void move(size_t plane, size_t cluster_id, larocv::data::ParticledQdX&& cluster);
      
    private:
      /// Unique ID that should be same as Vertex3D's id
      size_t _id;
      /// A container of clusters w/ outer index = plane, inner index = cluster id
      std::vector<std::vector<larocv::data::ParticledQdX> > _cluster_vv;
    };

    /**
       \class dQdXProfilerData
       @brief only for vic
    */
    class dQdXProfilerData : public AlgoDataBase {
      
    public:
    
      /// Default constructor
      dQdXProfilerData(std::string name="NoName", AlgorithmID_t id=0)
	: AlgoDataBase(name,id)
      {  Clear(); }    
      /// Default destructor
      ~dQdXProfilerData(){}
      /// Clear method override
      void Clear();

      /// register with Vertex3D ID using copy
      void insert(size_t vtx_id, const larocv::data::ParticledQdXArray& col);
      /// register with Vertex3D ID using move
      void move(size_t vtx_id, larocv::data::ParticledQdXArray&& col);
      /// accessor to vtx-wise ParticledQdXArray set
      const std::vector<larocv::data::ParticledQdXArray>& get_vertex_clusters() const;
      /// accessor to a single ParticledQdXArray ( clusters for 1 vertex )
      const larocv::data::ParticledQdXArray& get_vertex_cluster(size_t id) const;

    private:
      std::vector<larocv::data::ParticledQdXArray> _vtx_cluster_v;
      
    };
  }  
}
#endif
/** @} */ // end of doxygen group
