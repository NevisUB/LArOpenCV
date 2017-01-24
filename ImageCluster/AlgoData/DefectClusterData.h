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
    class ClusterCompoundArray;
    class ParticleCompoundArray;
    class DefectClusterData;
    
    class AtomicContour : public GEO2D_Contour_t {
      friend class ClusterCompound;
    public:
      
      AtomicContour(size_t atomic_id = kINVALID_SIZE)
	: GEO2D_Contour_t(), _atomic_id(atomic_id) { clear(); }
      AtomicContour(const GEO2D_Contour_t& ctor,size_t parent_idx,size_t atomic_id)
	: GEO2D_Contour_t(ctor)
	, _parent_idx(parent_idx)
	, _atomic_id(atomic_id)
      { clear(); }
      /*
      AtomicContour(AtomicContour&& rhs)
	: _ctor(std::move(rhs._ctor))
	, _parent_idx(rhs._parent_idx)
	, _atomic_id(rhs._atomic_id)
	, _defect_id_v(std::move(rhs._defect_id_v))
      {}
      */      
      ~AtomicContour(){}

      //Always protect existing class members
      AtomicContour& operator=(const GEO2D_Contour_t& ctor) {
	GEO2D_Contour_t::operator=(ctor);
	return *this;
      }

      AtomicContour& operator=(const AtomicContour& ctor) {
	GEO2D_Contour_t::operator=(ctor);
	return *this;
      }
      
      /// clears data
      void clear();
      /// returns unique atomic cluster id
      size_t id() const;
      
      size_t _parent_idx;
      /// asociate argument defect point
      void associate(const ContourDefect& ac);
      /// check if the argument defect point is associated
      bool is_associated(const ContourDefect& ac) const;
      /// retrieve a list of defect points associated
      const std::vector<size_t>& associated_defects() const;
      
    private:
      /// unique atomic cluster id
      size_t _atomic_id;
      /// associated defect point(s) id, use std::vector instead of std::set for Python-friendly support
      std::vector<size_t> _defect_id_v;  
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
      const std::vector<size_t>& associated_atoms() const;
      
    private:
      /// unique ID of a defect point
      size_t _defect_id; 
      /// associated ID of atomic clusters, use std::vector instead of std::set for Python-friendly support
      std::vector<size_t> _atomic_id_v; 
    };

    class ClusterCompound : public std::vector<AtomicContour> {
      friend class ClusterCompoundArray;
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
      /// save association
      void associate(size_t atom_id, size_t defect_id);
      /// retrieve a list of atomic clusters
      const std::vector<larocv::data::AtomicContour>& get_atoms() const;
      /// retrieve a list of defects
      const std::vector<larocv::data::ContourDefect>& get_defects() const;
      /// retrieve an atomic cluster by id
      const larocv::data::AtomicContour& get_atom(size_t id) const;
      /// retrieve a defect by id
      const larocv::data::ContourDefect& get_defect(size_t id) const;
      
    private:
      //std::vector< AtomicContour > _atomic_ctor_v; ///< a list of atomic cluster, ordered by their unique ID
      std::vector< ContourDefect > _ctor_defect_v; ///< a list of contour defect, ordered by their unique ID
      size_t _cluster_id; ///< unique ID associated with original cluster
    };
    
    /*
      \class ClusterCompoundArray
    */
    class ClusterCompoundArray {
    public:
      ClusterCompoundArray() { clear(); }
      ~ClusterCompoundArray() {}
      
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
      DefectClusterData()
      { Clear();}
      ~DefectClusterData(){}
      
      void Clear();
      
      std::vector<larocv::data::ClusterCompoundArray> _raw_cluster_vv;
      
      /*
      /// copy-add vertex-wise ParticleClusterArray compound information
      void push_back(const Vertex3D& vtx, const larocv::data::ParticleCompoundArray& col);
      /// move-add vertex-wise ParticleClusterArray compound information
      void emplace_back(const Vertex3D& vtx, larocv::data::ParticleCompoundArray&& col);
      */
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
