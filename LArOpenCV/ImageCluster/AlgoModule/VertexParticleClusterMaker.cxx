#ifndef __VERTEXPARTICLECLUSTERMAKER_CXX__
#define __VERTEXPARTICLECLUSTERMAKER_CXX__

#include "TrackVertexEstimate.h"
#include "VertexParticleClusterMaker.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

namespace larocv {

  /// Global larocv::VertexParticleClusterMakerFactory to register AlgoFactory
  static VertexParticleClusterMakerFactory __global_VertexParticleClusterMakerFactory__;
  
  void VertexParticleClusterMaker::_Configure_(const Config_t &pset) {
    
    _DefectBreaker.Configure(pset.get<Config_t>("DefectBreaker"));

    _VertexParticleCluster.set_verbosity(this->logger().level());
    _VertexParticleCluster.Configure(pset.get<Config_t>("VertexParticleCluster"));
    _VertexParticleCluster.PrintConfig();

    _pi_threshold = _VertexParticleCluster._pi_threshold;
    
    auto const vtx_algo_name = pset.get<std::string>("VertexEstimateAlgo");
    _vertex_estimate_algo_id = this->ID(vtx_algo_name);

    auto const scluster_algo_name = pset.get<std::string>("SuperClusterAlgo");
    _super_cluster_algo_id = this->ID(scluster_algo_name);

    _contour_pad = pset.get<float>("ContourPad",0.);

    _create_compound   = pset.get<bool>("CreateCompound",false);
    _min_particle_size = pset.get<uint>("MinParticleContourSize",3);

    for(size_t plane=0; plane<3; ++plane)
      Register(new data::ParticleClusterArray);

    if(_create_compound) {
      _DefectBreaker.set_verbosity(this->logger().level());
      _DefectBreaker.Configure(pset.get<Config_t>("DefectBreaker"));
      for(size_t plane=0; plane<3; ++plane)
	Register(new data::TrackClusterCompoundArray);
    }
  }

  void VertexParticleClusterMaker::_Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta,
				       larocv::ROI& roi)
  {
    auto const plane = meta.plane();

    // Algorithm data
    auto& par_data = AlgoData<data::ParticleClusterArray>(plane);
    
    const auto& vtx_data_v = AlgoData<data::Vertex3DArray>(_vertex_estimate_algo_id,0).as_vector();

    LAROCV_DEBUG() << "Got " << vtx_data_v.size() << " vertices" << std::endl;

    // Run clustering for this plane & store
    const auto& super_cluster_v = AlgoData<data::ParticleClusterArray>(_super_cluster_algo_id,plane).as_vector();

    LAROCV_DEBUG() << "Got " << super_cluster_v.size() << " super clusters on plane " << plane << std::endl;
    
    for(size_t vtx_id = 0; vtx_id < vtx_data_v.size(); ++vtx_id) {
      
      const auto& vtx3d = vtx_data_v[vtx_id];
      const auto& circle_vtx = vtx3d.cvtx2d_v.at(plane);

      LAROCV_DEBUG() << "Vertex ID " << vtx_id << " plane " << plane
		     << " CircleVertex @ " << circle_vtx.center << " w/ R = " << circle_vtx.radius << std::endl;

      if (circle_vtx.radius<=0) {
	LAROCV_DEBUG() << "This circle vertex is invalid, skip!" << std::endl;
	continue;
      }
      
      // Find corresponding super cluster, currently have to make a copy
      GEO2D_ContourArray_t super_ctor_v;
      super_ctor_v.reserve(super_cluster_v.size());
      for(const auto& super_cluster : super_cluster_v) {
	LAROCV_DEBUG() << "... setting " << super_cluster._ctor.size() << std::endl;
	  super_ctor_v.emplace_back(super_cluster._ctor);
      }
      auto const super_cluster_id = FindContainingContour(super_ctor_v, circle_vtx.center);
      
      if(super_cluster_id == kINVALID_SIZE) {
	LAROCV_WARNING() << "Skipping Vertex ID " << vtx_id << " on plane " << plane << " as no super-cluster found..." << std::endl;
	continue;
      }

      //this vertex is associated to this cluster
      LAROCV_DEBUG() << "Associating vertex " << vtx_id
		     << " with super cluster " << super_cluster_id
		     << std::endl;

      AssociateOne(vtx3d,super_cluster_v[super_cluster_id]); 
      
      // Create contours from the vertex point
      auto contour_v = _VertexParticleCluster.CreateParticleCluster(img,circle_vtx,super_cluster_v[super_cluster_id]);
      
      LAROCV_DEBUG() << "Found " << contour_v.size() << " contours for vertex id " << vtx_id << std::endl;
      
      for(size_t cidx=0; cidx<contour_v.size(); ++cidx) {
	auto& contour = contour_v[cidx];
	LAROCV_DEBUG() << "On contour: " << cidx << "... size: " << contour.size() << std::endl;
	
	if (contour.size() < _min_particle_size) {
	  LAROCV_DEBUG() << "... size of this contour too small. Skip!" << std::endl;
	  continue;
	}
	
	data::ParticleCluster cluster;
	cluster._ctor = std::move(contour);
	  
	// Store
	par_data.emplace_back(std::move(cluster));
	// Create one-to-many association
	AssociateMany(vtx3d,par_data.as_vector().back());

	// if defect is requested, let's make here
	if(_create_compound) {
	  auto& particle = par_data.as_vector().back();
	  // Create cluster compound
	  auto cluscomp = _DefectBreaker.BreakContour(particle._ctor);
	  // Order the atomics
	  auto const ordered_atom_id_v = _AtomicAnalysis.OrderAtoms(cluscomp,circle_vtx.center);
	  // Determine edges
	  auto atom_edges_v = _AtomicAnalysis.AtomsEdge(cluscomp, circle_vtx.center, ordered_atom_id_v);
	  // set start and end point per atomic
	  for(auto atom_id : ordered_atom_id_v) {
	    auto& atom = cluscomp.get_atomic(atom_id);
	    auto& start_end = atom_edges_v[atom_id];
	    atom.set_start(start_end.first);
	    atom.set_end(start_end.second);
	  }
	  // set the atomic order
	  cluscomp.set_atomic_order(ordered_atom_id_v);
	  // Get the farthest edge
	  auto& last_start_end = atom_edges_v.at(ordered_atom_id_v.back());
	  // Set the edge
	  cluscomp.set_end_pt(last_start_end.second);
	  // Store
	  auto& compound_v = AlgoData<data::TrackClusterCompoundArray>(3+plane);
	  compound_v.emplace_back(std::move(cluscomp));
	  AssociateMany(vtx3d,compound_v.as_vector().back());
	  AssociateOne(particle,compound_v.as_vector().back());
	}
      }
    }
    return;
  }
  
  
  bool VertexParticleClusterMaker::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    return true;
  } 
  
}
#endif

