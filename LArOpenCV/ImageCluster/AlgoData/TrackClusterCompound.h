#ifndef __TRACK_CLUSTER_COMPOUND_H__
#define __TRACK_CLUSTER_COMPOUND_H__

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "Geo2D/Core/Line.h"
/*
  @brief: storage for atomic clusters (AtomicContour), cluster compounds (TrackClusterCompound)
          and defects (ContourDefect)
*/
namespace larocv {
  namespace data {

    class AtomicContour;
    class ContourDefect;
    class TrackClusterCompound;
    
    class AtomicContour : public GEO2D_Contour_t {
      friend class TrackClusterCompound;
    public:
      AtomicContour()
	: GEO2D_Contour_t(), _atomic_id(kINVALID_SIZE) { clear(); }
      AtomicContour(size_t atomic_id)
	: GEO2D_Contour_t(), _atomic_id(atomic_id) { clear(); }
      AtomicContour(const GEO2D_Contour_t& ctor,size_t atomic_id)
	: GEO2D_Contour_t(ctor)
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
      /// query for ID
      size_t id() const;
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
      /// start point setter
      void set_start(geo2d::Vector<float> start);
      /// start point getter
      geo2d::Vector<float>& start_pt();
      /// end point setter
      void set_end(geo2d::Vector<float> end);
      /// end point getter
      geo2d::Vector<float>& end_pt();
      
    private:
      /// unique atomic cluster id
      size_t _atomic_id;
      /// associated defect point(s) id, use std::vector instead of std::set for Python-friendly support
      std::vector<size_t> _defect_id_v;
      /// edges vector
      std::vector<geo2d::Vector<float> > _edges_v;
      /// pca line
      geo2d::Line<float> _pca;
      /// dqdx
      std::vector<float> _dqdx_v;
      /// start
      geo2d::Vector<float> _start;
      /// end
      geo2d::Vector<float> _end;
    };
    
    class ContourDefect {
      friend class TrackClusterCompound;
    public:
      ContourDefect()
	: _defect_id(kINVALID_SIZE) {clear();}
      ContourDefect(size_t defect_id)
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

    class TrackClusterCompound: public AlgoDataArrayElementBase,
				public std::vector<AtomicContour> {
    public:
      
      TrackClusterCompound()
      {_Clear_();}
      ~TrackClusterCompound() {}
      
      /// clears data
      void _Clear_();
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
      /// retrieve an atomic cluster by id
      larocv::data::AtomicContour& get_atomic(size_t id);
      /// retrieve a defect by id
      const larocv::data::ContourDefect& get_defect(size_t id) const;
      /// sets the end point
      void set_end_pt(geo2d::Vector<float>& ep);
      /// queries the end point
      const geo2d::Vector<float>& end_pt() const;
    protected:
      std::vector<ContourDefect> _ctor_defect_v; ///< a list of contour defect, ordered by their unique ID
      std::vector<size_t> _ordered_index_v;
      geo2d::Vector<float> _end_pt;
    };

    /**
       \class TrackClusterCompoundArray
    */
    typedef AlgoDataArrayTemplate<TrackClusterCompound> TrackClusterCompoundArray;
    
  }
}
#endif
/** @} */ // end of doxygen group
