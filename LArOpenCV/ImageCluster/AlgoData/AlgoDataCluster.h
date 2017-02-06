#ifndef __ALGODATA_CLUSTER_H__
#define __ALGODATA_CLUSTER_H__

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "Geo2D/Core/Line.h"

namespace larocv {
  namespace data {

    class ClusterCompound;
    class ClusterCompoundArray;
    class ContourDefect;
    
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
      ~AtomicContour(){}

      //Always protect existing class members upon assignment
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
      /// add an edge
      void add_edge(geo2d::Vector<float>& edge);
      /// get all edges as ref
      std::vector<geo2d::Vector<float> >& edges();
      /// pca line setter
      void set_pca(const geo2d::Line<float>& line);
      /// pca line getter
      const geo2d::Line<float>& pca();
      /// dqdx setter
      void set_dqdx(const std::vector<float>& dqdx);
      /// dqdx getter
      const std::vector<float>& dqdx();
      
    private:
      /// unique atomic cluster id
      size_t _atomic_id;
      /// associated defect point(s) id, use std::vector instead of std::set for Python-friendly support
      std::vector<size_t> _defect_id_v;
      /// edges vector
      std::vector<geo2d::Vector<float> > _edges_v;
      /// pca line
      geo2d::Line<float> _pca;
      /// dQ/dX array
      std::vector<float>  _dqdx_v;
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
      /// set the atomic ordering
      void set_atomic_order(std::vector<size_t> atom_idx_v);
      /// retreive the ordered atomics
      std::vector<larocv::data::AtomicContour*> get_ordered_atoms();
      /// retrieve a list of defects
      const std::vector<larocv::data::ContourDefect>& get_defects() const;
      /// retrieve an atomic cluster by id
      const larocv::data::AtomicContour& get_atom(size_t id) const;
      /// retrieve a defect by id
      const larocv::data::ContourDefect& get_defect(size_t id) const;

      // /// queries dq/dx index array that corresponds to atoms' start position
      // const std::vector<size_t>& atom_start_index_array() const;
      // /// queries atomic contours' id in the order added for dqdx construction
      // const std::vector<size_t>& atom_id_array() const;
      // /// queries 2D coordinate point array that corresponds atoms' start position
      // const std::vector<geo2d::Vector<float> >& atom_start_pt_array() const;
      // /// queries the dq/dx index that corresponds to an atom's start position
      // size_t atom_start_index(size_t atom_index) const;
      // /// queries atomic contour's id
      // size_t atom_id(size_t atom_index) const;
      // /// queries 2D coordinate point that corresponds to the start of the chain of atoms
      // const geo2d::Vector<float>& start_pt() const;
      // /// queries 2D coordinate point that corresponds to an atom's start position
      // const geo2d::Vector<float>& atom_start_pt(size_t atom_index) const;

      /// sets the end point
      void set_end_pt(geo2d::Vector<float>& ep);
      /// queries the end point
      const geo2d::Vector<float>& end_pt() const;

      // /// queries the end point
      // const geo2d::Vector<float>& atom_end_pt(const size_t atom_index) const;
      // /// resolution of dx
      // const double dx_resolution() const { return _dx_resolution; }
      
    protected:
      std::vector<ContourDefect> _ctor_defect_v; ///< a list of contour defect, ordered by their unique ID
      size_t _cluster_id; ///< unique ID associated with original cluster
      std::vector<size_t> _ordered_index_v;
      // /// unique ID (should respect Particle cluster id)
      // // size_t _cluster_id;
      // /// resolution of dx
      // double _dx_resolution;
      // /// atomic cluster's boundary (start) location in terms of dq/dx array index
      // std::vector<size_t> _start_index_v;
      // /// atomic cluster's boundary (start) location in 2D coordinate system
      // std::vector<geo2d::Vector<float> > _start_pt_v;

      /// end point of ALL atoms' chain
      geo2d::Vector<float> _end_pt;

      // /// atomic cluster's id array
      // std::vector<size_t> _atom_id_v;
      
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

    
  }
}
#endif
/** @} */ // end of doxygen group
