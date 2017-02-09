#ifndef __TRACKPARTICLECLUSTER_CXX__
#define __TRACKPARTICLECLUSTER_CXX__

#include "TrackVertexEstimate.h"
#include "TrackParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ContourArrayData.h"

namespace larocv {

  /// Global larocv::TrackParticleClusterFactory to register AlgoFactory
  static TrackParticleClusterFactory __global_TrackParticleClusterFactory__;
  
  void TrackParticleCluster::_Configure_(const Config_t &pset) {
    
    _DefectBreaker.Configure(pset);
    _AtomicAnalysis.Configure(pset);

    _VertexParticleCluster.set_verbosity(this->logger().level());
    _VertexParticleCluster.Configure(pset.get<Config_t>("VertexParticleCluster"));
    _VertexParticleCluster.PrintConfig();

    _pi_threshold = _VertexParticleCluster._pi_threshold;
    
    auto const vtx_algo_name = pset.get<std::string>("TrackVertexEstimateAlgo");
    _track_vertex_estimate_algo_id = this->ID(vtx_algo_name);

    auto const scluster_algo_name = pset.get<std::string>("SuperClusterAlgo");
    _track_super_cluster_algo_id = this->ID(scluster_algo_name);

    _contour_pad = pset.get<float>("ContourPad",0.);

    for(size_t plane=0; plane<3; ++plane)
      Register(new data::ParticleClusterArray);
  }

  void TrackParticleCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta,
				       larocv::ROI& roi)
  {
    auto const plane = meta.plane();

    // Algorithm data
    auto& par_data = AlgoData<data::ParticleClusterArray>(plane);
    
    const auto& vtx_data_v = AlgoData<data::Vertex3DArray>(_track_vertex_estimate_algo_id,0).as_vector();

    LAROCV_DEBUG() << "Got " << vtx_data_v.size() << " vertices" << std::endl;
    for(const auto& vtx : vtx_data_v)
      LAROCV_DEBUG() << "... id " << vtx.ID() << std::endl;
      
    // Run clustering for this plane & store
    auto const& super_cluster_v = AlgoData<data::ContourArrayData>(_track_super_cluster_algo_id,plane);

    LAROCV_DEBUG() << "Got " << super_cluster_v.size() << " super clusters on plane " << plane << std::endl;
    
    for(size_t vtx_id = 0; vtx_id < vtx_data_v.size(); ++vtx_id) {
      
      auto const& vtx3d = vtx_data_v[vtx_id];
      auto const& circle_vtx = vtx3d.cvtx2d_v.at(plane);
      LAROCV_DEBUG() << "Vertex ID " << vtx_id << " plane " << plane
		     << " CircleVertex @ " << circle_vtx.center << " w/ R = " << circle_vtx.radius << std::endl;

      // Find corresponding super cluster
      auto const super_cluster_id = FindContainingContour(super_cluster_v, circle_vtx.center);
      if(super_cluster_id == kINVALID_SIZE) {
	LAROCV_WARNING() << "Skipping Vertex ID " << vtx_id << " on plane " << plane << " as no super-cluster found..." << std::endl;
	continue;
      }
      
      // Create track contours from the vertex point
      auto contour_v = _VertexParticleCluster.CreateParticleCluster(img,circle_vtx,super_cluster_v[super_cluster_id]);

      LAROCV_DEBUG() << "Found " << contour_v.size() << " contours for vertex id " << vtx_id << std::endl;
      
      for(size_t cidx=0; cidx<contour_v.size(); ++cidx) {
	auto& contour = contour_v[cidx];
	LAROCV_DEBUG() << "On contour: " << cidx << "... size: " << contour.size() << std::endl;
	data::ParticleCluster cluster;
	cluster._ctor = std::move(contour);
	// Store
	par_data.emplace_back(std::move(cluster));
	// Create one-to-many association
	AssociateMany(vtx3d,par_data.as_vector().back());
      }
    }
    
    return;
  }
  
  
  bool TrackParticleCluster::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {

    return true;
  } 
  
}
#endif

