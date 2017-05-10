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
    
    auto const scluster_algo_name = pset.get<std::string>("SuperClusterAlgo");
    _super_cluster_algo_id = this->ID(scluster_algo_name);
    if(_super_cluster_algo_id==kINVALID_ALGO_ID)
      throw larbys("Unknown SuperClusterAlgo");

    // Create a container for this algorithm data
    for(size_t plane=0; plane<3; ++plane) Register(new data::ParticleClusterArray);
    
  }

  void VertexParticleClusterMaker::_Process_()
  {
    //
    // 0) Prep
    //
    auto img_v = ImageArray();
    auto const& meta_v = MetaArray();

    for(size_t img_idx=0; img_idx<img_v.size(); ++img_idx) {

      auto& img = img_v[img_idx];
      auto const& meta = meta_v.at(img_idx);
      auto const plane = meta.plane();
      
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
	    
      // Get associated super clusters on this plane
      const auto& super_cluster_v = AlgoData<data::ParticleClusterArray>(_super_cluster_algo_id,plane).as_vector();
      LAROCV_DEBUG() << "Got " << super_cluster_v.size() << " ADC super clusters on plane " << plane << std::endl;

      GEO2D_ContourArray_t super_ctor_v;
      super_ctor_v.reserve(super_cluster_v.size());
      for(const auto& super_cluster : super_cluster_v)
	super_ctor_v.emplace_back(super_cluster._ctor);

      //
      // 1) Find particle clusters with the VertexParticleCluster module
      //
      
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

	const auto& super_cluster = super_cluster_v[super_cluster_id];
	
	// This vertex is associated to this cluster
	LAROCV_DEBUG() << "Associating vertex " << vertex_id << " with super cluster " << super_cluster_id << std::endl;
	AssociateOne(*vertex3d,super_cluster_v[super_cluster_id]); 
	
	// Create particle clusters @ this circle vertex and parent super cluster
	auto contour_v = _VertexParticleCluster.CreateParticleCluster(img,circle_vertex,super_cluster);
	
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

