#ifndef __ALGODATAUTILS_CXX__
#define __ALGODATAUTILS_CXX__

#include "AlgoDataUtils.h"
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/VectorAnalysis.h"
#include <array>
#include <cassert>

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

    Vertex3D
    Difference(const Vertex3D& vtx1, const Vertex3D& vtx2) {
      Vertex3D res;

      res.x = vtx2.x - vtx1.x;
      res.y = vtx2.y - vtx1.y;
      res.z = vtx2.z - vtx1.z;
      
      return res;
    }
    
    double
    Distance(const Vertex3D& vtx1, const Vertex3D& vtx2) {
      return std::sqrt(std::pow(vtx1.x-vtx2.x,2)+
		       std::pow(vtx1.y-vtx2.y,2)+
		       std::pow(vtx1.z-vtx2.z,2));
    }

    bool
    Equals(const Vertex3D& vtx1, const Vertex3D& vtx2) {
      if(vtx1.x          != vtx2.x)          return false;
      if(vtx1.y          != vtx2.y)          return false;
      if(vtx1.z          != vtx2.z)          return false;
      if(vtx1.type       != vtx2.type)       return false;
      if(vtx1.num_planes != vtx2.num_planes) return false;

      if (vtx1.vtx2d_v.size()!=vtx2.vtx2d_v.size()) return false;

      for(size_t plane=0;plane<vtx1.vtx2d_v.size();++plane)
	if (vtx1.vtx2d_v[plane].pt!=vtx2.vtx2d_v[plane].pt) return false;

      return true;
    }
    
    std::vector<Vertex2D>
    Seed2Vertex(const std::vector<VertexSeed2D>& svtx2d_v) {
      std::vector<Vertex2D> res;
      res.reserve(svtx2d_v.size());
      for(const auto& svtx2d : svtx2d_v)
	res.emplace_back(Seed2Vertex(svtx2d));
      return res;
    }

    Vertex2D
    Seed2Vertex(const VertexSeed2D& svtx2d) {
      Vertex2D res;
      res.pt.x = svtx2d.x;
      res.pt.y = svtx2d.y;
      return res;
    }
      
    Vertex3D
    Seed2Vertex(const VertexSeed3D& svtx3d) {
      Vertex3D res;
      res.vtx2d_v = Seed2Vertex(svtx3d.vtx2d_v);
      res.x = svtx3d.x;
      res.y = svtx3d.y;
      res.z = svtx3d.z;
      res.type = VertexType_t::kUnknown;
      return res;
    }

    std::pair<float,float> Angle3D(const std::vector<data::SpacePt>& sps_v,
				   const data::Vertex3D& start3d) {
      
      static data::Vertex3D mean_pt;
      return Angle3D(sps_v,start3d,mean_pt);
    }

    std::pair<float,float> Angle3D(const std::vector<data::SpacePt>& sps_v,
				   const data::Vertex3D& start3d,
				   data::Vertex3D& mean_pt) {
      
    
      cv::Mat vertex_mat(sps_v.size(), 3, CV_32FC1);
    
      for(size_t vtxid=0; vtxid<sps_v.size(); ++vtxid) {
	vertex_mat.at<float>(vtxid, 0) = sps_v.at(vtxid).pt.x;
	vertex_mat.at<float>(vtxid, 1) = sps_v.at(vtxid).pt.y;
	vertex_mat.at<float>(vtxid, 2) = sps_v.at(vtxid).pt.z;
      }

      LAROCV_SDEBUG() << "Calculating PCA for " << vertex_mat.rows << " 3D points" << std::endl;
      cv::PCA pca_ana(vertex_mat, cv::Mat(), CV_PCA_DATA_AS_ROW, 0);

      std::array<float,3> mean_v;
      std::array<float,3> eigen_v;

      for(size_t plane=0; plane<3; ++plane) {
	mean_v[plane] = pca_ana.mean.at<float>(0,plane);
	eigen_v[plane]= pca_ana.eigenvectors.at<float>(0,plane);
      }

      auto eigen_len = std::sqrt( eigen_v[0] * eigen_v[0] +
				  eigen_v[1] * eigen_v[1] +
				  eigen_v[2] * eigen_v[2] );
    
    
      LAROCV_SDEBUG() << "PCA @ ("<<mean_v[0]<<","<<mean_v[1]<<","<<mean_v[2]
		     <<") dir: ("<<eigen_v[0]<<","<<eigen_v[1]<<","<<eigen_v[2]<<")"<<std::endl;
    

      // check if incoming start point is valid
      if (start3d.x != kINVALID_DOUBLE) {
	// determine if we should flip the eigen direction based on the 3D mean position
	assert(start3d.x != kINVALID_DOUBLE);
	assert(start3d.y != kINVALID_DOUBLE);
	assert(start3d.z != kINVALID_DOUBLE);

	std::array<float,3> mean_dir_v;
	mean_dir_v[0] = mean_v[0] - start3d.x;
	mean_dir_v[1] = mean_v[1] - start3d.y;
	mean_dir_v[2] = mean_v[2] - start3d.z;
	auto mean_dir_len = std::sqrt( mean_dir_v[0] * mean_dir_v[0] +
				       mean_dir_v[1] * mean_dir_v[1] +
				       mean_dir_v[2] * mean_dir_v[2] );
	mean_dir_v[0] /= mean_dir_len;
	mean_dir_v[1] /= mean_dir_len;
	mean_dir_v[2] /= mean_dir_len;

	//
	// implement direction handling, negative dot product, flip the sign on eigen
	//

	auto dot_product = Dot(mean_dir_v,eigen_v);
	if (dot_product < 0) {
	  eigen_v[0] *= -1;
	  eigen_v[1] *= -1;
	  eigen_v[2] *= -1;
	}
      }

      mean_pt.x = mean_v[0];
      mean_pt.y = mean_v[1];
      mean_pt.z = mean_v[2];
    
      auto cos = eigen_v[2] / eigen_len;
      //auto tan = eigen_v[1] / eigen_v[0];

      auto arccos = std::acos(cos);
      auto arctan = std::atan2(eigen_v[1],eigen_v[0]);

      LAROCV_SDEBUG() << "rad: theta="<<arccos<<" phi="<<arctan<<std::endl;
      LAROCV_SDEBUG() << "deg: theta="<<arccos*180.0/3.14<<" phi="<<arctan*180.0/3.14<<std::endl;
      return std::make_pair(arccos,arctan);
    }

    std::pair<float,float> Angle3D(const data::Vertex3D& vtx1,
				   const data::Vertex3D& vtx2) {
      
      LAROCV_SDEBUG() << "Angle 3D from 2 vertex" << std::endl;
      auto res_dist = Distance(vtx1,vtx2);
      auto res_vtx  = Difference(vtx1,vtx2);
      
      if (res_dist == 0) throw larbys("Vertex1 and Vertex2 cannot be the same");
      
      auto cos = res_vtx.z / res_dist;
      //auto tan = res_vtx.y / res_vtx.x;
      
      auto arccos = std::acos(cos);
      auto arctan = std::atan2(res_vtx.y,res_vtx.x);
      
      LAROCV_SDEBUG() << "rad: theta="<<arccos<<" phi="<<arctan<<std::endl;
      LAROCV_SDEBUG() << "deg: theta="<<arccos*180/3.14<<" phi="<<arctan*180/3.14<<std::endl;
      return std::make_pair(arccos,arctan);
    }  

  }
}
#endif
