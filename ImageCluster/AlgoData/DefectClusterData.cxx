#ifndef __ALGODATA_DEFECTCLUSTERDATA_CXX__
#define __ALGODATA_DEFECTCLUSTERDATA_CXX__

#include "DefectClusterData.h"
#include "Core/larbys.h"

namespace larocv {

  namespace data {

    void AtomicContour::clear()
    { _ctor.clear(); _parent_idx = -1; _defect_id_s.clear(); }
    
    size_t AtomicContour::id() const
    { return _atomic_id; }
    
    void AtomicContour::associate(const ContourDefect& def)
    { _defect_id_s.insert(def.id()); }
    
    bool AtomicContour::is_associated(const ContourDefect& def) const
    { return (_defect_id_s.find(def.id()) != _defect_id_s.end()); }
    
    const std::set<size_t> AtomicContour::associated_defects() const
    { return _defect_id_s; }
    
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
      _atomic_id_s.clear();
    }
    
    size_t ContourDefect::id() const
    { return _defect_id; }
    
    void ContourDefect::associate(const AtomicContour& ac)
    { _atomic_id_s.insert(ac.id()); }
    
    bool ContourDefect::is_associated(const AtomicContour& ac) const
    { return (_atomic_id_s.find(ac.id()) != _atomic_id_s.end()); }
    
    const std::set<size_t> ContourDefect::associated_atoms() const
    { return _atomic_id_s; }
    
    ////////////////////////////////////////////////////////////////
    
    void ClusterCompound::clear()
    { _atomic_ctor_v.clear(); _ctor_defect_v.clear(); }
    
    size_t ClusterCompound::id() const
    { return _cluster_id; }
    
    void ClusterCompound::insert(const AtomicContour& atom)
    {
      if(atom.id() == kINVALID_SIZE) throw larbys("Cannot add an atom with invalid id...");
      if(_atomic_ctor_v.size() <= atom.id()) _atomic_ctor_v.resize(atom.id()+1);
      _atomic_ctor_v[atom.id()] = atom;
    }
    
    void ClusterCompound::insert(const ContourDefect& defect)
    {
      if(defect.id() == kINVALID_SIZE) throw larbys("Cannot add an defect with invalid id...");
      if(_ctor_defect_v.size() <= defect.id()) _ctor_defect_v.resize(defect.id()+1);
      _ctor_defect_v[defect.id()] = defect;
    }
    
    void ClusterCompound::move(AtomicContour&& atom)
    {
      if(atom.id() == kINVALID_SIZE) throw larbys("Cannot add an atom with invalid id...");
      if(_atomic_ctor_v.size() <= atom.id()) _atomic_ctor_v.resize(atom.id()+1);
      _atomic_ctor_v[atom.id()] = std::move(atom);
    }
    
    void ClusterCompound::move(ContourDefect&& defect)
    {
      if(defect.id() == kINVALID_SIZE) throw larbys("Cannot add an defect with invalid id...");
      if(_ctor_defect_v.size() <= defect.id()) _ctor_defect_v.resize(defect.id()+1);
      _ctor_defect_v[defect.id()] = std::move(defect);
    }
    
    const std::vector<larocv::data::AtomicContour>& ClusterCompound::get_atoms() const
    { return _atomic_ctor_v; }
    
    const std::vector<larocv::data::ContourDefect>& ClusterCompound::get_defects() const
    { return _ctor_defect_v; }
    
    const larocv::data::AtomicContour& ClusterCompound::get_atom(size_t id) const
    {
      if(id >= _atomic_ctor_v.size()) throw larbys("Invalid atomic id requested");
      return _atomic_ctor_v[id];
    }
    
    const larocv::data::ContourDefect& ClusterCompound::get_defect(size_t id) const
    {
      if(id >= _ctor_defect_v.size()) throw larbys("Invalid defect id requested");
      return _ctor_defect_v[id];
    }
    
    ////////////////////////////////////////////////////////////////
    
    void DefectClusterPlaneData::clear()
    { _cluster_v.clear(); }

    size_t DefectClusterPlaneData::num_clusters() const
    { return _cluster_v.size(); }
    
    const std::vector<larocv::data::ClusterCompound>& DefectClusterPlaneData::get_cluster() const
    { return _cluster_v; }
    
    const larocv::data::ClusterCompound& DefectClusterPlaneData::get_cluster(size_t id) const
    {
      if(id >= _cluster_v.size()) throw larbys("Invalid ClusterCompound ID requested");
      return _cluster_v[id];
    }
    
    void DefectClusterPlaneData::set_num_clusters(size_t n)
    { _cluster_v.resize(n); }
    
    void DefectClusterPlaneData::insert(const ClusterCompound& c)
    {
      if(c.id() == kINVALID_SIZE) throw larbys("Cannot insert ClusterCompound with an invalid id");
      if(c.id() >= _cluster_v.size()) _cluster_v.resize(c.id()+1);
      _cluster_v[c.id()] = c;
    }
    
    void DefectClusterPlaneData::move(ClusterCompound&& c)
    {
      if(c.id() == kINVALID_SIZE) throw larbys("Cannot insert ClusterCompound with an invalid id");
      if(c.id() >= _cluster_v.size()) _cluster_v.resize(c.id()+1);
      _cluster_v[c.id()] = std::move(c);
    }
    
    ///////////////////////////////////////////////////////////////
    
    void DefectClusterData::Clear()
    {
      _plane_data.clear();
      _plane_data.resize(3);
    }
  }
}
#endif
