#ifndef __ALGODATA_CLUSTER_CXX__
#define __ALGODATA_CLUSTER_CXX__

#include "AlgoDataCluster.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {
  namespace data {

    ///////////////////////////////////////////////
    void AtomicContour::clear()
    { GEO2D_Contour_t::clear(); _parent_idx = -1; _defect_id_v.clear(); _edges_v.clear(); }
    
    size_t AtomicContour::id() const
    { return _atomic_id; }
    
    void AtomicContour::associate(const ContourDefect& def)
    { if( ! is_associated(def) ) _defect_id_v.push_back( def.id() ); }
    
    bool AtomicContour::is_associated(const ContourDefect& def) const
    {
      for(auto const& did : _defect_id_v) {
	if(did == def.id()) return true;
      }
      return false;
    }
    const std::vector<size_t>& AtomicContour::associated_defects() const
    { return _defect_id_v; }

    void AtomicContour::set_pca(const geo2d::Line<float>& line)
    {  _pca = line; }

    const geo2d::Line<float>& AtomicContour::pca()
      { return _pca; }
    
    void AtomicContour::add_edge(geo2d::Vector<float>& edge)
    { _edges_v.push_back(edge); }
    
    std::vector<geo2d::Vector<float>>& AtomicContour::edges()
    { return _edges_v; }

    /// dqdx setter
    void AtomicContour::set_dqdx(const std::vector<float>& dqdx) {
      _dqdx_v = dqdx;
    }
    /// dqdx getter
    const std::vector<float>& AtomicContour::dqdx() {
      return _dqdx_v;
    }

    ContourDefect::ContourDefect(const geo2d::Vector<int>& pt_start,
				 const geo2d::Vector<int>& pt_end,
				 const geo2d::Vector<int>& pt_defect,
				 float dist,
				 const geo2d::Line<float>& split_line,
				 size_t defect_id = kINVALID_SIZE)
      : _pt_start(pt_start)
      , _pt_end(pt_end)
      , _pt_defect(pt_defect)
      , _dist(dist)
      , _split_line(split_line)
      , _defect_id(defect_id)
    {}
    
    void ContourDefect::clear()
    {
      _pt_start.x  = _pt_start.y  = -1;
      _pt_end.x    = _pt_end.y    = -1;
      _pt_defect.x = _pt_defect.y = -1;
      _dist = -1;
      _split_line.pt.x  = _split_line.pt.y  = -1;
      _split_line.dir.x = _split_line.dir.y = -1;
      _atomic_id_v.clear();
    }
    
    size_t ContourDefect::id() const
    { return _defect_id; }
    
    void ContourDefect::associate(const AtomicContour& ac)
    { if(!is_associated(ac)) _atomic_id_v.push_back(ac.id()); }
    
    bool ContourDefect::is_associated(const AtomicContour& ac) const
    {
      for(auto const& aid : _atomic_id_v) {
	if(aid == ac.id()) return true;
      }
      return false;
    }
    
    const std::vector<size_t>& ContourDefect::associated_atoms() const
    { return _atomic_id_v; }

    
    ///////////////////////////////////////////////
    void ClusterCompound::clear()
    { std::vector<AtomicContour>::clear(); _ctor_defect_v.clear(); }
    
    size_t ClusterCompound::id() const
    { return _cluster_id; }

    AtomicContour& ClusterCompound::make_atom()
    {
      this->push_back(AtomicContour());
      this->back()._atomic_id = this->size() - 1;
      return this->back();
    }

    ContourDefect& ClusterCompound::make_defect()
    {
      _ctor_defect_v.push_back(ContourDefect());
      _ctor_defect_v.back()._defect_id = _ctor_defect_v.size() - 1;
      return _ctor_defect_v.back();
    }
    
    void ClusterCompound::push_back(const AtomicContour& atom)
    {
      std::vector<AtomicContour>::push_back(atom);
      this->back()._atomic_id = this->size() - 1;
    }
    
    void ClusterCompound::push_back(const ContourDefect& defect)
    {
      _ctor_defect_v.push_back(defect);
      _ctor_defect_v.back()._defect_id = _ctor_defect_v.size() - 1;
    }
    
    void ClusterCompound::emplace_back(AtomicContour&& atom)
    {
      std::vector<AtomicContour>::emplace_back(std::move(atom));
      this->back()._atomic_id = this->size() - 1;
    }
    
    void ClusterCompound::emplace_back(ContourDefect&& defect)
    {
      _ctor_defect_v.emplace_back(std::move(defect));
      _ctor_defect_v.back()._defect_id = _ctor_defect_v.size() - 1;
    }

    void ClusterCompound::associate(size_t atom_id, size_t defect_id)
    {
      if( atom_id   >= this->size() ) throw larbys("Cannot associate invalid atomic id!");
      if( defect_id >= _ctor_defect_v.size() ) throw larbys("Cannot associate invalid defect id!");

      auto& atomic_ctor = this->      at( atom_id   );
      auto& ctor_defect = _ctor_defect_v[ defect_id ];

      if( atomic_ctor.id() != atom_id   ) throw larbys("Found inconsistent atomic id and index location!");
      if( ctor_defect.id() != defect_id ) throw larbys("Found inconsistent defect id and index location!");

      atomic_ctor.associate(ctor_defect);
      ctor_defect.associate(atomic_ctor);
    }
    
    const std::vector<AtomicContour>& ClusterCompound::get_atoms() const
    { return *this; }
    
    const std::vector<ContourDefect>& ClusterCompound::get_defects() const
    { return _ctor_defect_v; }
    
    const AtomicContour& ClusterCompound::get_atom(size_t id) const
    {
      if(id >= this->size()) throw larbys("Invalid atomic id requested");
      return this->at(id);
    }
    
    const ContourDefect& ClusterCompound::get_defect(size_t id) const
    {
      if(id >= _ctor_defect_v.size()) throw larbys("Invalid defect id requested");
      return _ctor_defect_v[id];
    }

    /// sets the end point
    void ClusterCompound::set_end_pt(geo2d::Vector<float>& ep) {
      _end_pt = ep;
    }
    /// queries the end point
    const geo2d::Vector<float>& ClusterCompound::end_pt() const {
      return _end_pt;
    }
    /// set the atomic ordering
    void ClusterCompound::set_atomic_order(std::vector<size_t> atom_idx_v) {
      _ordered_index_v = atom_idx_v;
    }

    /// retrieve a list of atomic clusters
    std::vector<AtomicContour*> ClusterCompound::get_ordered_atoms(){

      if (_ordered_index_v.empty())
	throw larbys("Atomic ordering not set");
      
      std::vector<AtomicContour*> ordered_atomics;
      ordered_atomics.resize(this->size());

      for(size_t idx=0;idx<ordered_atomics.size();++idx)
	ordered_atomics[idx] = & ( (*this)[ _ordered_index_v[idx] ] );
      
      return ordered_atomics;
      
    }

    
    void ClusterCompoundArray::clear()
    { _cluster_v.clear(); }

    size_t ClusterCompoundArray::num_clusters() const
    { return _cluster_v.size(); }

    size_t ClusterCompoundArray::num_defects() const
    {
      size_t res=0;
      for(auto const& c : _cluster_v) res += c.get_defects().size();
      return res;
    }

    
    size_t ClusterCompoundArray::num_atoms() const
    {
      size_t res=0;
      for(auto const& c : _cluster_v) res += c.get_atoms().size();
      return res;
    }

    const std::vector<ClusterCompound>& ClusterCompoundArray::get_cluster() const
    { return _cluster_v; }
    
    const ClusterCompound& ClusterCompoundArray::get_cluster(size_t id) const
    {
      if(id >= _cluster_v.size()) throw larbys("Invalid ClusterCompound ID requested");
      return _cluster_v[id];
    }
    
    void ClusterCompoundArray::push_back(const ClusterCompound& c)
    {
      _cluster_v.push_back(c);
      _cluster_v.back()._cluster_id = _cluster_v.size()-1;
    }
    
    void ClusterCompoundArray::emplace_back(ClusterCompound&& c)
    {
      _cluster_v.emplace_back(std::move(c));
      _cluster_v.back()._cluster_id = _cluster_v.size() - 1;
    }

    ClusterCompound& ClusterCompoundArray::make_cluster()
    {
      _cluster_v.push_back(ClusterCompound());
      _cluster_v.back()._cluster_id = _cluster_v.size() - 1;
      return _cluster_v.back();
    }
    
  }
}
#endif
