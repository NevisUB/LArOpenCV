#ifndef __ALGODATA_DEFECTCLUSTERDATA_H__
#define __ALGODATA_DEFECTCLUSTERDATA_H__

#include "Base/AlgoDataBase.h"
#include "Core/Line.h"
#include <set>

namespace larocv {

  namespace data {

    class ContourDefect;
    
    class AtomicContour {
    public:
      
      AtomicContour(size_t atomic_id = kINVALID_SIZE)
	: _atomic_id(atomic_id) { clear(); }
      AtomicContour(const GEO2D_Contour_t& ctor,size_t parent_idx,size_t atomic_id)
	: _ctor(ctor)
	, _parent_idx(parent_idx)
	, _atomic_id(atomic_id)
      {clear();}
      ~AtomicContour(){}
      
      /// clears data
      void clear();
      /// returns unique atomic cluster id
      size_t id() const;
      
      GEO2D_Contour_t _ctor;
      size_t _parent_idx;
      
      /// asociate argument defect point
      void associate(const ContourDefect& ac);
      /// check if the argument defect point is associated
      bool is_associated(const ContourDefect& ac) const;
      /// retrieve a list of defect points associated
      const std::set<size_t> associated_defects() const;
      
    private:
      size_t _atomic_id; ///< unique atomic cluster id
      std::set<size_t> _defect_id_s;  ///< associated defect point(s) id
    };
    
    class ContourDefect {
    public:
      
      ContourDefect(size_t defect_id = kINVALID_SIZE)
	: _defect_id(defect_id) {clear();}
      ContourDefect(const geo2d::Vector<int>& pt_start,
		    const geo2d::Vector<int>& pt_end,
		    const geo2d::Vector<int>& pt_defect,
		    float dist,
		    const geo2d::Line<float>& split_line,
		    size_t defect_id);
      ~ContourDefect() {}
      
      /// clears data
      void clear();
      /// returns unique defect id
      size_t id() const;
      
      geo2d::Vector<float> _pt_start;
      geo2d::Vector<float> _pt_end;
      geo2d::Vector<float> _pt_defect;
      float _dist;
      geo2d::Line<float> _split_line;
      
      /// asociate argument atomic cluster
      void associate(const AtomicContour& ac);
      /// check if the argument atomic cluster is associated
      bool is_associated(const AtomicContour& ac) const;
      /// retrieve a list of atomic clusters associated
      const std::set<size_t> associated_atoms() const;
      
    private:
      size_t _defect_id; ///< unique ID of a defect point
      std::set<size_t> _atomic_id_s; ///< associated ID of atomic clusters
    };
    
    class ClusterCompound {
    public:
      
      ClusterCompound(size_t id=kINVALID_SIZE)
	: _cluster_id(id) {clear();}
      ~ClusterCompound() {}
      
      /// clears data
      void clear();
      /// returns unique cluster id
      size_t id() const;
      
      /// copy-add atomic cluster
      void insert(const AtomicContour& atom);
      /// copy-add defect
      void insert(const ContourDefect& defect);
      /// in-place-add atomic cluster
      void move(AtomicContour&& atom);
      /// in-place-add defect
      void move(ContourDefect&& defect);
      /// retrieve a list of atomic clusters
      const std::vector<larocv::data::AtomicContour>& get_atoms() const;
      /// retrieve a list of defects
      const std::vector<larocv::data::ContourDefect>& get_defects() const;
      /// retrieve an atomic cluster by id
      const larocv::data::AtomicContour& get_atom(size_t id) const;
      /// retrieve a defect by id
      const larocv::data::ContourDefect& get_defect(size_t id) const;
      
    private:
      std::vector< AtomicContour > _atomic_ctor_v; ///< a list of atomic cluster, ordered by their unique ID
      std::vector< ContourDefect > _ctor_defect_v; ///< a list of contour defect, ordered by their unique ID
      size_t _cluster_id; ///< unique ID associated with original cluster
    };
    
    /*
      \class DefectClusterPlaneData
    */
    class DefectClusterPlaneData {
    public:
      DefectClusterPlaneData() { clear(); }
      ~DefectClusterPlaneData() {}
      
      /// Attribute clear method
      void clear();
      /// number of currently registered clusters
      size_t num_clusters() const;
      /// retrieve a list of clusters on this plane
      const std::vector<larocv::data::ClusterCompound>& get_cluster() const;
      /// retrieve a list of clusters on this plane via id
      const larocv::data::ClusterCompound& get_cluster(size_t id) const;
      /// reset total number of clusters on this plane
      void set_num_clusters(size_t n);
      /// copy-insert a cluster
      void insert(const ClusterCompound& c);
      /// move-insert a cluster
      void move(ClusterCompound&& c);
      
    private:
      
      /// list of cluster compounds
      std::vector<larocv::data::ClusterCompound> _cluster_v;
    };

    /**
       \class ParticleClusterDefectArray
       @brief per-vertex collection container for track clusters' defects across planes
    */
    class ParticleCompoundArray {
    public:
      ParticleCompoundArray(size_t id=kINVALID_SIZE) : _id(id)
      { clear(); }
      ~ParticleCompoundArray() {}
      //
      // accessors
      //
      /// get id (from Vertex3D)
      size_t id() const;
      /// get # of planes
      size_t num_planes() const;
      /// get # clusters
      size_t num_clusters(size_t plane) const;
      /// Plane-wise cluster list accessor
      const std::vector<larocv::data::ClusterCompound>& get_compound(size_t plane) const;
      //
      // modifiers
      //
      /// clear data attributes
      void clear();
      /// set # of planes
      void num_planes(size_t n);
      /// Cluster (per plane) appender
      void insert(size_t plane, const larocv::data::ClusterCompound& cluster);
      /// Cluster (per plane) appender
      void move(size_t plane, larocv::data::ClusterCompound&& cluster);
      
    private:
      ///< ID that should correspond to Vertex3D
      size_t _id;
      ///< an array of 2D clusters (per plane)
      std::vector<std::vector< larocv::data::ClusterCompound > > _cluster_vv;
    };

    /*
      \class DefectClusterData
    */
    class DefectClusterData : public larocv::data::AlgoDataBase {
    public:
      DefectClusterData(std::string name="NoName", AlgorithmID_t id=0)
	: AlgoDataBase(name,id)
      { Clear();}
      ~DefectClusterData(){}
      
      void Clear();
      
      std::vector<larocv::data::DefectClusterPlaneData> _plane_data;
      std::vector<larocv::data::ParticleCompoundArray> _vtx_cluster_v;
    };
  }
}
#endif
