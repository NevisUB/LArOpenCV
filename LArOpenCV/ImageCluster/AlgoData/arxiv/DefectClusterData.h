#ifndef __ALGODATA_DEFECTCLUSTERDATA_H__
#define __ALGODATA_DEFECTCLUSTERDATA_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "AlgoDataCluster.h"

namespace larocv {

  namespace data {

    // forward declaration for friends
    class ParticleCompoundArray;
    class DefectClusterData;

    /**
       \class ParticleClusterDefectArray
       @brief per-vertex collection container for track clusters' defects across planes
    */
    class ParticleCompoundArray{
      friend class DefectClusterData;
    public:
      ParticleCompoundArray(size_t id=kINVALID_SIZE) : _id(id)
      { clear(); }
      ~ParticleCompoundArray() {}
      //
      // accessors
      //
      /// get id 
      size_t id() const;
      /// get # of planes
      size_t num_planes() const;
      /// get # clusters
      size_t num_clusters(size_t plane) const;
      /// Plane-wise cluster list accessor
      const std::vector<larocv::data::ClusterCompound>& get_cluster(size_t plane) const;
      /// Plane-wise cluster generation
      larocv::data::ClusterCompound& make_cluster(size_t plane);
      //
      // modifiers
      //
      /// clear data attributes
      void clear();
      /// set # of planes
      void num_planes(size_t n);
      /// Cluster (per plane) appender
      void push_back(size_t plane, const larocv::data::ClusterCompound& cluster);
      /// Cluster (per plane) appender
      void emplace_back(size_t plane, larocv::data::ClusterCompound&& cluster);
      
    private:
      ///< ID 
      size_t _id;
      ///< an array of 2D clusters (per plane)
      std::vector<std::vector< larocv::data::ClusterCompound > > _cluster_vv;
    };

    /*
      \class DefectClusterData
    */
    class DefectClusterData : public larocv::data::AlgoDataBase {
    public:
      DefectClusterData()
      { Clear();}
      ~DefectClusterData(){}
      
      void Clear();
      
      std::vector<larocv::data::ClusterCompoundArray> _raw_cluster_vv;
      /// query # of vertex clusters (ParticleCompoundArray vector size)
      size_t num_vertex_clusters() const { return _vtx_cluster_v.size();}
      /// register with Vertex3D ID using copy
      void insert(size_t vtx_id, const larocv::data::ParticleCompoundArray& col);
      /// register with Vertex3D ID using move
      void move(size_t vtx_id, larocv::data::ParticleCompoundArray&& col);
      /// accessor to vtx-wise ParticleCompoundArray set
      const std::vector<larocv::data::ParticleCompoundArray>& get_vertex_clusters() const;
      /// accessor to a single ParticleCompoundArray ( clusters for 1 vertex )
      const larocv::data::ParticleCompoundArray& get_vertex_cluster(size_t id) const;
      
    private:
      std::vector<larocv::data::ParticleCompoundArray> _vtx_cluster_v;
      
    };
  }
}
#endif
