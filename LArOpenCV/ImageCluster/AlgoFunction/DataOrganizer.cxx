#ifndef __DATAORGANIZER_CXX__
#define __DATAORGANIZER_CXX__

#include "DataOrganizer.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {
  namespace data {

    std::vector<VertexTrackInfoCollection>
    OrganizeVertexInfo(const AlgoDataAssManager& ass_man,
		       const Vertex3DArray& vtx3d_array,
		       std::vector<const ParticleClusterArray*>& super_cluster_array_v,
		       std::vector<const ParticleClusterArray*>& part_cluster_array_v,
		       std::vector<const TrackClusterCompoundArray*>& compound_array_v)
    {
      std::vector<VertexTrackInfoCollection> res_v;
      auto const& vtx3d_v = vtx3d_array.as_vector();
      if(vtx3d_v.empty()) return res_v;

      const size_t num_planes = vtx3d_v.front().num_planes;
      
      if(num_planes != super_cluster_array_v.size()) {
	LAROCV_SCRITICAL() << "# planes != super_cluster_v array size..." << std::endl;
	throw larbys();
      }

      if(num_planes != compound_array_v.size()) {
	LAROCV_SCRITICAL() << "# planes != compound_v array size..." << std::endl;
	throw larbys();
      }
      
      if(num_planes != part_cluster_array_v.size()) {
	LAROCV_SCRITICAL() << "# planes != part_cluster_v array size..." << std::endl;
	throw larbys();
      }

      res_v.resize(vtx3d_v.size());
      for(size_t idx=0; idx<vtx3d_v.size(); ++idx) {
	auto& res = res_v[idx];
	// Vertex3D
	res.vtx3d = &(vtx3d_v[idx]);

	// Plane wise business
	auto const& vtx3d = vtx3d_v[idx];
	for(size_t plane=0; plane<num_planes; ++plane) {

	  auto const& super_cluster_array = super_cluster_array_v[plane];
	  auto const& part_cluster_array  = part_cluster_array_v[plane];
	  auto const& compound_array      = compound_array_v[plane];

	  if(!super_cluster_array)
	    res.super_contour_v.push_back(nullptr);
	  else{
	    auto ass_idx = ass_man.GetOneAss(vtx3d,super_cluster_array->ID());
	    res.super_contour_v.push_back(&(super_cluster_array->as_vector().at(ass_idx)));
	  }

	  std::vector<const ParticleCluster*> part_ptr_v;
	  if(!part_cluster_array)
	    res.particle_v.push_back(part_ptr_v);
	  else{
	    auto ass_idx_v = ass_man.GetManyAss(vtx3d,part_cluster_array->ID());
	    for(auto const& cluster_idx : ass_idx_v)
	      part_ptr_v.push_back(&(part_cluster_array->as_vector().at(cluster_idx)));
	    res.particle_v.push_back(part_ptr_v);
	  }

	  std::vector<const TrackClusterCompound*> compound_ptr_v;
	  if(!compound_array)
	    res.compound_v.push_back(compound_ptr_v);
	  else{
	    auto ass_idx_v = ass_man.GetManyAss(vtx3d,compound_array->ID());
	    for(auto const& cluster_idx : ass_idx_v)
	      compound_ptr_v.push_back(&(compound_array->as_vector().at(cluster_idx)));
	    res.compound_v.push_back(compound_ptr_v);
	  }
	}
      }
      return res_v;
    }
  }
}
#endif
