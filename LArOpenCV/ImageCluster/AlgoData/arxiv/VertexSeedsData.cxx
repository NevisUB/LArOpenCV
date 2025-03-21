#ifndef __ALGODATA_VERTEXSEEDSDATA_CXX__
#define __ALGODATA_VERTEXSEEDSDATA_CXX__

#include "VertexSeedsData.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {

  namespace data {

    void VertexSeeds::store_compound(const ClusterCompound& compound) {
      _cluscomp_v.emplace_back(compound);
    }
    void VertexSeeds::store_compound(ClusterCompound&& compound) {
      _cluscomp_v.emplace_back(std::move(compound));
    }

    void VertexSeeds::store_pcaxs(const std::vector<geo2d::Vector<float> >& pcaxs_v) {
      if ( _pcaxs_v.empty() )  {
	_pcaxs_v = pcaxs_v;
      } else {
	_pcaxs_v.reserve(_pcaxs_v.size() + pcaxs_v.size());
	_pcaxs_v.insert(std::end(_pcaxs_v),
			std::begin(pcaxs_v),
			std::end(pcaxs_v));
      }
    }
    
    void VertexSeeds::store_pcaxs(std::vector<geo2d::Vector<float> >&& pcaxs_v) {
      if (_pcaxs_v.empty()) {
	_pcaxs_v = std::move(pcaxs_v);
      } else {
	_pcaxs_v.reserve(_pcaxs_v.size() + pcaxs_v.size());
	_pcaxs_v.insert(std::end(_pcaxs_v),
			std::make_move_iterator(std::begin(pcaxs_v)),
			std::make_move_iterator(std::end(pcaxs_v)));
      }
    }
    
    const VertexSeeds& VertexSeedsData::get_seeds(short plane_id) const {
      if (plane_id<0) throw larbys("Plane ID must be greater than 0");
      if (plane_id>3) throw larbys("Plane ID must be less than 3");
      return _vertex_seeds_v[plane_id];
    }
    
    void VertexSeedsData::plant_seeds(const VertexSeeds& vtx_seeds, short plane_id) {
      if (plane_id<0) throw larbys("Plane ID must be greater than 0");
      if (plane_id>3) throw larbys("Plane ID must be less than 3");
      _vertex_seeds_v[plane_id] = vtx_seeds;
    }
    void VertexSeedsData::plant_seeds(VertexSeeds&& vtx_seeds, short plane_id) {
      if (plane_id<0) throw larbys("Plane ID must be greater than 0");
      if (plane_id>3) throw larbys("Plane ID must be less than 3");
      _vertex_seeds_v[plane_id] = std::move(vtx_seeds);
    }


    const VertexSeeds& VertexSeedsData::harvest_seeds(short plane_id) const {
      return _vertex_seeds_v[plane_id];
    }
    
    std::vector<geo2d::Vector<float> > VertexSeedsData::harvest_seed_points(short plane_id) const{
      const auto& vtx_seed = _vertex_seeds_v[plane_id];
      auto& pcaxs_v = vtx_seed._pcaxs_v;
      auto& cluscomp_v = vtx_seed._cluscomp_v;
      
      std::vector<geo2d::Vector<float> > points;
      points.reserve(cluscomp_v.size()*3 + pcaxs_v.size());

      points = pcaxs_v;      
      
      for(const auto& compound : cluscomp_v)
	for(const auto& ctor_defect : compound.get_defects())
	  points.emplace_back(ctor_defect._pt_defect);
      
      return points;
    }
    
  }
}
#endif
