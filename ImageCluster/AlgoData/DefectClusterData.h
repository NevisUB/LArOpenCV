#ifndef __ALGODATA_DEFECTCLUSTERDATA_H__
#define __ALGODATA_DEFECTCLUSTERDATA_H__

#include "Base/AlgoDataBase.h"
#include "Core/Line.h"
#include <set>

namespace larocv {

  namespace data {

    // forward declaration for friends
    class ContourDefect;
    class ClusterCompound;
    class DefectClusterPlaneData;
    class ParticleCompoundArray;
    class DefectClusterData;
    
    class AtomicContour {
      friend class ClusterCompound;
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
      friend class ClusterCompound;
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
      friend class DefectClusterPlaneData;
      friend class ParticleCompoundArray;
    public:
      
      ClusterCompound(size_t id=kINVALID_SIZE)
	: _cluster_id(id) {clear();}
      ~ClusterCompound() {}
      
      /// clears data
      void clear();
      /// returns unique cluster id
      size_t id() const;

      /// generate new atomic cluster w/ valid id
      AtomicContour& make_atom();
      /// generate new defect w/ valid id
      ContourDefect& make_defect();
      /// copy-add atomic cluster
      void push_back(const AtomicContour& atom);
      /// copy-add defect
      void push_back(const ContourDefect& defect);
      /// in-place-add atomic cluster
      void emplace_back(AtomicContour&& atom);
      /// in-place-add defect
      void emplace_back(ContourDefect&& defect);
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
      /// total number of defects
      size_t num_defects() const;
      /// total number of atoms
      size_t num_atoms() const;
      /// retrieve a list of clusters on this plane
      const std::vector<larocv::data::ClusterCompound>& get_cluster() const;
      /// retrieve a list of clusters on this plane via id
      const larocv::data::ClusterCompound& get_cluster(size_t id) const;
      /// generate a new cluster container
      larocv::data::ClusterCompound& make_cluster();
      /// copy-insert a cluster
      void push_back(const ClusterCompound& c);
      /// move-insert a cluster
      void emplace_back(ClusterCompound&& c);
      
    private:
      
      /// list of cluster compounds
      std::vector<larocv::data::ClusterCompound> _cluster_v;
    };

    /**
       \class ParticleClusterDefectArray
       @brief per-vertex collection container for track clusters' defects across planes
    */
    class ParticleCompoundArray {
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
      DefectClusterData(std::string name="NoName", AlgorithmID_t id=0)
	: AlgoDataBase(name,id)
      { Clear();}
      ~DefectClusterData(){}
      
      void Clear();
      
      std::vector<larocv::data::DefectClusterPlaneData> _plane_data;

      /// copy-add vertex-wise ParticleClusterArray compound information
      void push_back(const larocv::data::ParticleCompoundArray& col);
      /// move-add vertex-wise ParticleClusterArray compound information
      void emplace_back(const larocv::data::ParticleCompoundArray& col);
      /// insert vertex-wise ParticleClusterArray compound specifying vertex id to be used as index
      void insert(size_t vtx_id, const larocv::data::ParticleCompoundArray& col);
      
    private:
      std::vector<larocv::data::ParticleCompoundArray> _vtx_cluster_v;
    };
  }
}
#endif
