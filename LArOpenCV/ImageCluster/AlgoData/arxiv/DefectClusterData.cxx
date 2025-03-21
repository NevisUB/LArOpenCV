#ifndef __ALGODATA_DEFECTCLUSTERDATA_CXX__
#define __ALGODATA_DEFECTCLUSTERDATA_CXX__

#include "DefectClusterData.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {

  namespace data {

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
    
    ////////////////////////////////////////////////////////////////
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
    
    ////////////////////////////////////////////////////////////////
    
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
    
    const std::vector<larocv::data::AtomicContour>& ClusterCompound::get_atoms() const
    { return *this; }
    
    const std::vector<larocv::data::ContourDefect>& ClusterCompound::get_defects() const
    { return _ctor_defect_v; }
    
    const larocv::data::AtomicContour& ClusterCompound::get_atom(size_t id) const
    {
      if(id >= this->size()) throw larbys("Invalid atomic id requested");
      return this->at(id);
    }
    
    const larocv::data::ContourDefect& ClusterCompound::get_defect(size_t id) const
    {
      if(id >= _ctor_defect_v.size()) throw larbys("Invalid defect id requested");
      return _ctor_defect_v[id];
    }
    
    ////////////////////////////////////////////////////////////////
    
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

    const std::vector<larocv::data::ClusterCompound>& ClusterCompoundArray::get_cluster() const
    { return _cluster_v; }
    
    const larocv::data::ClusterCompound& ClusterCompoundArray::get_cluster(size_t id) const
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

    larocv::data::ClusterCompound& ClusterCompoundArray::make_cluster()
    {
      _cluster_v.push_back(ClusterCompound());
      _cluster_v.back()._cluster_id = _cluster_v.size() - 1;
      return _cluster_v.back();
    }

    ///////////////////////////////////////////////////////////////

    size_t ParticleCompoundArray::id() const
    { return _id; }

    size_t ParticleCompoundArray::num_planes() const
    { return _cluster_vv.size(); }

    size_t ParticleCompoundArray::num_clusters(size_t plane) const
    {
      return get_cluster(plane).size();
    }

    const std::vector<larocv::data::ClusterCompound>& ParticleCompoundArray::get_cluster(size_t plane) const
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      return _cluster_vv[plane];
    }

    void ParticleCompoundArray::clear()
    {
      _cluster_vv.clear();
      _cluster_vv.resize(3);
    }
    
    void ParticleCompoundArray::push_back(size_t plane, const larocv::data::ClusterCompound& cluster)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      cluster_v.push_back(cluster);
      cluster_v.back()._cluster_id = cluster_v.size() - 1;
    }

    void ParticleCompoundArray::emplace_back(size_t plane, larocv::data::ClusterCompound&& cluster)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      cluster_v.emplace_back(std::move(cluster));
      cluster_v.back()._cluster_id = cluster_v.size() - 1;
    }

    larocv::data::ClusterCompound& ParticleCompoundArray::make_cluster(size_t plane)
    {
      if(plane >= _cluster_vv.size()) throw larbys("Invalid plane requested!");
      auto& cluster_v = _cluster_vv[plane];
      cluster_v.push_back(ClusterCompound());
      cluster_v.back()._cluster_id = cluster_v.size() - 1;
      return cluster_v.back();
    }
    
    ///////////////////////////////////////////////////////////////
    
    void DefectClusterData::Clear()
    {
      _raw_cluster_vv.clear();
      _raw_cluster_vv.resize(3);
      _vtx_cluster_v.clear();
    }
    /*
      void DefectClusterData::push_back(const larocv::data::ParticleCompoundArray& col)
      {
      _vtx_cluster_v.push_back(col);
      _vtx_cluster_v.back()._id = _vtx_cluster_v.size() - 1;
      }

      void DefectClusterData::emplace_back(larocv::data::ParticleCompoundArray&& col)
      {
      _vtx_cluster_v.emplace_back(std::move(col));
      _vtx_cluster_v.back()._id = _vtx_cluster_v.size() - 1;
      }
    */
    void DefectClusterData::insert(size_t vtx_id, const larocv::data::ParticleCompoundArray& col)
    {
      while(_vtx_cluster_v.size() <= vtx_id) {
	_vtx_cluster_v.push_back(ParticleCompoundArray());
	_vtx_cluster_v.back()._id = _vtx_cluster_v.size() - 1;
      }
      _vtx_cluster_v[vtx_id] = col;
      _vtx_cluster_v[vtx_id]._id = vtx_id;
    }
    
    void DefectClusterData::move(size_t vtx_id, larocv::data::ParticleCompoundArray&& col)
    {
      while(_vtx_cluster_v.size() <= vtx_id) {
	_vtx_cluster_v.push_back(ParticleCompoundArray());
	_vtx_cluster_v.back()._id = _vtx_cluster_v.size() - 1;
      }
      _vtx_cluster_v[vtx_id] = std::move(col);
      _vtx_cluster_v[vtx_id]._id = vtx_id;
    }

    const std::vector<larocv::data::ParticleCompoundArray>& DefectClusterData::get_vertex_clusters() const
    { return _vtx_cluster_v; }

    const larocv::data::ParticleCompoundArray& DefectClusterData::get_vertex_cluster(size_t vtx_id) const
    {
      if(vtx_id >= _vtx_cluster_v.size()) throw larbys("Invalid vertex id requested!");
      return _vtx_cluster_v[vtx_id];
    }



  }
}
#endif
