#ifndef __ALGODATAUTILS_CXX__
#define __ALGODATAUTILS_CXX__

#include "AlgoDataUtils.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {
  namespace data {

    std::vector<VertexTrackInfoCollection>
    OrganizeVertexInfo(const AlgoDataAssManager& ass_man,
		       const Vertex3DArray& vtx3d_array,
		       std::vector<const ParticleClusterArray*> super_cluster_array_v,
		       std::vector<const ParticleClusterArray*> particle_array_v,
		       std::vector<const TrackClusterCompoundArray*> compound_array_v)
    {
      std::vector<VertexTrackInfoCollection> res_v;
      auto const& vtx3d_v = vtx3d_array.as_vector();
      if(vtx3d_v.empty()) return res_v;

      const size_t num_planes = vtx3d_v.front().vtx2d_v.size();
      
      if(!super_cluster_array_v.empty() && num_planes != super_cluster_array_v.size()) {
      	LAROCV_SCRITICAL() << "# planes " << num_planes << " != super_cluster_v array size " << super_cluster_array_v.size() << std::endl;
      	throw larbys();
      }

      if(!compound_array_v.empty() && num_planes != compound_array_v.size()) {
      	LAROCV_SCRITICAL() << "# planes " << num_planes << " != compound_v array size " << compound_array_v.size() << std::endl;
      	throw larbys();
      }
      
      if(!particle_array_v.empty() && num_planes != particle_array_v.size()) {
      	LAROCV_SCRITICAL() << "# planes " << num_planes << " != particle_v array size " << particle_array_v.size() << std::endl;
      	throw larbys();
      }

      LAROCV_SINFO() << "Organizing " << vtx3d_v.size() << " vertices" << std::endl;
      res_v.resize(vtx3d_v.size());
      for(size_t idx=0; idx<vtx3d_v.size(); ++idx) {
	auto& res = res_v[idx];
	// Vertex3D

	res.vtx3d = &(vtx3d_v[idx]);
	LAROCV_SINFO() << "Inspecting track vertex " << idx << " @ (" << vtx3d_v[idx].x << "," << vtx3d_v[idx].y << "," << vtx3d_v[idx].z << ")" << std::endl;
	LAROCV_SINFO() << res.vtx3d->vtx2d_v.size() << " number of planes" << std::endl;

	// Plane wise business
	auto const& vtx3d = vtx3d_v[idx];
	LAROCV_SDEBUG() << "This vertex " << &vtx3d << std::endl;
	for(size_t plane=0; plane<num_planes; ++plane) {
	  LAROCV_SDEBUG() << "... @ plane " << plane << std::endl;
	  if(!super_cluster_array_v.empty()) {
	    auto const& super_cluster_array = super_cluster_array_v[plane];
	    if(!super_cluster_array or super_cluster_array->as_vector().empty()) {
	      res.super_contour_v.push_back(nullptr);
	      LAROCV_SDEBUG() << " ... no super cluster exists" << std::endl;
	    } else {
	      auto ass_idx = ass_man.GetOneAss(vtx3d,super_cluster_array->ID());
	      if (ass_idx==kINVALID_SIZE)  {
		LAROCV_SCRITICAL() << ".. associated super cluster index can not be found for vertex " << idx << std::endl;
		throw larbys();
	      }
	      LAROCV_SDEBUG() << "... ass index " << ass_idx << " is found" << std::endl;
	      res.super_contour_v.push_back(&(super_cluster_array->as_vector().at(ass_idx)));
	    }
	  }
	  bool process_particle = (!particle_array_v.empty() && particle_array_v[plane]);
	  bool process_compound = (!compound_array_v.empty() && compound_array_v[plane]);
	  std::vector<const ParticleCluster*>      particle_ptr_v;
	  std::vector<const TrackClusterCompound*> compound_ptr_v;

	  if(!process_particle && !process_compound) {
	    if(!particle_array_v.empty())
	      res.particle_vv.push_back(particle_ptr_v);
	    if(!compound_array_v.empty())
	      res.compound_vv.push_back(compound_ptr_v);
	    continue;
	  }

	  if(!process_particle) {
	    // part_cluster_array not needed, let's get compound sorted
	    if(!particle_array_v.empty())
	      res.particle_vv.push_back(particle_ptr_v);
	    auto const& compound_array = compound_array_v[plane];
	    auto ass_idx_v = ass_man.GetManyAss(vtx3d,compound_array->ID());
	    for(auto const& cluster_idx : ass_idx_v)
	      compound_ptr_v.push_back(&(compound_array->as_vector().at(cluster_idx)));
	    res.compound_vv.push_back(compound_ptr_v);
	    continue;
	  }
	  if(!process_compound) {
	    // compound_array is not needed, let's get particle contour sorted
	    if(!compound_array_v.empty())
	      res.compound_vv.push_back(compound_ptr_v);
	    auto const& particle_array  = particle_array_v[plane];
	    auto ass_idx_v = ass_man.GetManyAss(vtx3d,particle_array->ID());
	    for(auto const& cluster_idx : ass_idx_v)
	      particle_ptr_v.push_back(&(particle_array->as_vector().at(cluster_idx)));
	    res.particle_vv.push_back(particle_ptr_v);
	    continue;
	  }

	  // Reaching here means to treat both particle & compound
	  // Since both are given, let's make sure array order is 1-to-1	  
	  if(!particle_array_v.empty() && !compound_array_v.empty()) {
	    
	    auto const& particle_array  = particle_array_v[plane];
	    auto const& compound_array  = compound_array_v[plane];

	    auto const& compound_ass = ass_man.GetManyAss(vtx3d,compound_array->ID());
	    auto const& particle_ass = ass_man.GetManyAss(vtx3d,particle_array->ID());
	    if(compound_ass.size() != particle_ass.size()) {
	      LAROCV_SCRITICAL() << "Vertex3D (ID=" << vtx3d.ID() << ") have different number of associated"
				 << " particles (" << particle_ass.size() << ") and"
				 << " compounds (" << compound_ass.size() << ")!" << std::endl;
	      throw larbys();
	    }
	    for(auto const& compound_idx : compound_ass) {
	      auto const& compound = compound_array->as_vector().at(compound_idx);
	      auto particle_idx = ass_man.GetOneAss(compound,particle_array->ID());
	      // make sure particle index is found in retrieved association
	      bool found=false;
	      for(auto const& particle_ass_idx : particle_ass) {
		if(particle_ass_idx == particle_idx) {
		  found=true;
		  break;
		}
	      }
	      if(!found) {
		LAROCV_SCRITICAL() << "Compound/Particle association is corrupted?!" << std::endl;
		throw larbys();
	      }
	      particle_ptr_v.push_back(&(particle_array->as_vector().at(particle_idx)));
	      compound_ptr_v.push_back(&(compound_array->as_vector().at(compound_idx)));
	    }
	    res.particle_vv.push_back(particle_ptr_v);
	    res.compound_vv.push_back(compound_ptr_v);
	  }
	}
      }
      
      for(size_t idx=0; idx<res_v.size(); ++idx) {
	auto& res = res_v[idx];
	LAROCV_SINFO() << "Finished inspection of track vertex " << idx
		       << " @ (" << res.vtx3d->x << "," << res.vtx3d->y << "," << res.vtx3d->z << ") w/ nplanes " << res.vtx3d->vtx2d_v.size() << std::endl;
	LAROCV_SDEBUG() << "... at address " << res.vtx3d << std::endl;
      }
      
      return res_v;
    }
  }
}
#endif
