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

      size_t id() const;

      void push_back(const std::vector<float>& dqdx);
      
      void emplace_back(const std::vector<float>& dqdx);

    private:
      
      size_t _cluster_id;
      std::vector<float>  _dqdx_v;
      std::vector<size_t> _defects_v;
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
      /// Cluster (per plane) appender
      void push_back(size_t plane, const larocv::data::ParticledQdX& cluster);
      /// Cluster (per plane) appender
      void emplace_back(size_t plane, larocv::data::ParticledQdX&& cluster);
      
    private:
      size_t _id;
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
