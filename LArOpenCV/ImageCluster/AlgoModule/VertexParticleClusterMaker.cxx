#ifndef __VERTEXPARTICLECLUSTERMAKER_CXX__
#define __VERTEXPARTICLECLUSTERMAKER_CXX__

#include "TrackVertexEstimate.h"
#include "VertexParticleClusterMaker.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"


namespace larocv {

  /// Global larocv::VertexParticleClusterMakerFactory to register AlgoFactory
  static VertexParticleClusterMakerFactory __global_VertexParticleClusterMakerFactory__;
  
  void VertexParticleClusterMaker::_Configure_(const Config_t &pset) {

    //
    // Prepare algorithms via config
    //

    // Class to cluster particles in 2D
    _VertexParticleCluster.set_verbosity(this->logger().level());
    _VertexParticleCluster.Configure(pset.get<Config_t>("VertexParticleCluster"));
    _VertexParticleCluster.PrintConfig();

    _pi_threshold = _VertexParticleCluster._pi_threshold;

    // Track vertex estimator
    auto const track_vertex_algo_name = pset.get<std::string>("TrackVertexEstimateAlgo","");
    _track_vertex_estimate_algo_id = kINVALID_ALGO_ID;
    if (!track_vertex_algo_name.empty()) {
    _track_vertex_estimate_algo_id = this->ID(track_vertex_algo_name);
    if(_track_vertex_estimate_algo_id==kINVALID_ALGO_ID)
      throw larbys("Unknown TrackVertexEstimateAlgo");
    }

    // Shower vertex estimator
    auto const shower_vertex_algo_name = pset.get<std::string>("ShowerVertexEstimateAlgo","");
    _shower_vertex_estimate_algo_id = kINVALID_ALGO_ID;
    if (!shower_vertex_algo_name.empty()) {
      _shower_vertex_estimate_algo_id = this->ID(shower_vertex_algo_name);
      if(_shower_vertex_estimate_algo_id==kINVALID_ALGO_ID)
	throw larbys("Unknown ShowerVertexEstimateAlgo");
    }
    
    // Configure the SuperCluster finding algorithm via pset
    _SuperClusterer.Configure(pset.get<Config_t>("SuperClusterer"));
    
    // Create a container for this algorithm data
    for(size_t plane=0; plane<3; ++plane) Register(new data::ParticleClusterArray);

    _patch_dead_wire = pset.get<bool>("PatchImageDeadWire",false);
    _patch_linearity = pset.get<bool>("PatchClusters",false);

    if(_patch_dead_wire)
      _DeadWirePatch.Configure(pset.get<Config_t>("DeadWirePatch"));

    if(_patch_linearity)
      _ClustersPatch.Configure(pset.get<Config_t>("ClustersPatch"));
  }

  void VertexParticleClusterMaker::_Process_()
  {
    //
    // 0) Prep
    //
    auto img_v = ImageArray();
    std::vector<cv::Mat> dead_img_v;
    if (_patch_dead_wire) dead_img_v = ImageArray(ImageSetID_t::kImageSetChStatus);

    auto const& meta_v = MetaArray();
    
    
    for(size_t img_idx=0; img_idx<img_v.size(); ++img_idx) {

      const auto& img = img_v[img_idx];

      auto const& meta = meta_v.at(img_idx);
      auto const plane = meta.plane();

      cv::Mat mod_img;

      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss0;
	ss0 << "orig_plane_"<<plane<<"_input.png";
	cv::imwrite(std::string(ss0.str()).c_str(), img);
      }
      
      if (_patch_dead_wire)  { 
	const auto& dead_img = dead_img_v[img_idx];
	mod_img = _DeadWirePatch.Patch(img,dead_img);
      }
      else { mod_img = img; }

      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss0;
	ss0 << "fill_dead_plane_"<<plane<<"_output.png";
	cv::imwrite(std::string(ss0.str()).c_str(), mod_img);
      }

      if(_patch_linearity) {
	auto clusters_img = _ClustersPatch.AnglePatch(mod_img);
	mod_img = clusters_img;
      }

      if(this->logger().level() == ::larocv::msg::kDEBUG) {
	std::stringstream ss0;
	ss0 << "fill_clusters_plane_"<<plane<<"_output.png";
	cv::imwrite(std::string(ss0.str()).c_str(), mod_img);
      }
            
      // This algorithm data
      auto& par_data = AlgoData<data::ParticleClusterArray>(plane);

      // Previous algorithm vertex data
      std::vector<const data::Vertex3D*> vertex_data_v;
      
      if(_track_vertex_estimate_algo_id!=kINVALID_ALGO_ID) {
	const auto& track_vertex_data_v  = AlgoData<data::Vertex3DArray>(_track_vertex_estimate_algo_id,0).as_vector();
	for(const auto& vtx : track_vertex_data_v)
	  vertex_data_v.push_back(&vtx);
      }
      
      if(_shower_vertex_estimate_algo_id!=kINVALID_ALGO_ID) {
	const auto& shower_vertex_data_v = AlgoData<data::Vertex3DArray>(_shower_vertex_estimate_algo_id,0).as_vector();
	for(const auto& vtx : shower_vertex_data_v)
	  vertex_data_v.push_back(&vtx);
      }
	    
      GEO2D_ContourArray_t super_ctor_v;
      _SuperClusterer.CreateSuperCluster(mod_img,super_ctor_v);
      
      // For each vertex, get the 2D projection on this plane
      for(size_t vertex_id = 0; vertex_id < vertex_data_v.size(); ++vertex_id) {
	
	const auto& vertex3d = vertex_data_v[vertex_id];
	const auto& circle_vertex = vertex3d->cvtx2d_v.at(plane);
	
	LAROCV_DEBUG() << "Vertex ID " << vertex_id
		       << " plane " << plane
		       << " CircleVertex @ " << circle_vertex.center
		       << " w/ R = " << circle_vertex.radius << std::endl;
	
	if (circle_vertex.radius <= 0) {
	  LAROCV_DEBUG() << "This circle vertex is invalid, skip!" << std::endl;
	  continue;
	}
	
	// Find corresponding super cluster, to this projected vertex, if no super clusters exists, skip
	const auto super_cluster_id = FindContainingContour(super_ctor_v, circle_vertex.center);
	if(super_cluster_id == kINVALID_SIZE) continue;

	const auto& super_cluster = super_ctor_v[super_cluster_id];
	
	// This vertex is associated to this cluster
	// LAROCV_DEBUG() << "Associating vertex " << vertex_id << " with super cluster " << super_cluster_id << std::endl;
	// Create particle clusters @ this circle vertex and parent super cluster
	auto contour_v = _VertexParticleCluster.CreateParticleCluster(mod_img,circle_vertex,super_cluster);
	
	LAROCV_DEBUG() << "Found " << contour_v.size() << " contours for vertex id " << vertex_id << std::endl;
	for(size_t cidx=0; cidx<contour_v.size(); ++cidx) {
	  auto& contour = contour_v[cidx];
	  LAROCV_DEBUG() << "On contour: " << cidx << "... size: " << contour.size() << std::endl;
	  
	  // Make a new ParticleCluster
	  data::ParticleCluster cluster;
	  cluster._ctor = std::move(contour);
	  
	  // Store & make association to this vertex
	  par_data.emplace_back(std::move(cluster));
	  AssociateMany(*vertex3d,par_data.as_vector().back());
	   
	} // end this particle cluster
      } // end input vertex

      LAROCV_DEBUG() << std::endl;
    } // end this plane
    return;
  }
  
  bool VertexParticleClusterMaker::_PostProcess_() const
  { return true; }
  
}
#endif

