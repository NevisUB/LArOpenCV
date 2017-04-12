#ifndef __DEFECTCLUSTER_CXX__
#define __DEFECTCLUSTER_CXX__

#include "DefectCluster.h"
#include "Geo2D/Core/Geo2D.h"
#include "VertexTrackCluster.h"
#include "LinearTrackCluster.h"

namespace larocv {

  /// Global DefectClusterFactory to register AlgoFactory
  static DefectClusterFactory __global_DefectClusterFactory__;

  void DefectCluster::_Configure_(const Config_t &pset)
  {
    // Configure DefectBreaker
    _DefectBreaker.Configure(pset);
    
    auto const vertextrack_algo_name = pset.get<std::string>("VertexTrackClusterAlgo","");
    _vertextrack_algo_id = kINVALID_ALGO_ID;
    if(!vertextrack_algo_name.empty()) {
      LAROCV_INFO() << "Using vertex track cluster algo: " << vertextrack_algo_name << std::endl;
      _vertextrack_algo_id = this->ID(vertextrack_algo_name);
    }else{
      LAROCV_INFO() << "Not using vertex track cluster algo..." << std::endl;
    }

    auto const lineartrack_algo_name = pset.get<std::string>("LinearTrackClusterAlgo","");
    _lineartrack_algo_id = kINVALID_ALGO_ID;
    if(!lineartrack_algo_name.empty()) {
      LAROCV_INFO() << "Using linear track cluster algo: " << lineartrack_algo_name << std::endl;
      _lineartrack_algo_id = this->ID(lineartrack_algo_name);
    }else{
      LAROCV_INFO() << "Not using linear track cluster algo..." << std::endl;
    }

    Register(new data::DefectClusterData);
  }

  Cluster2DArray_t DefectCluster::_Process_(const Cluster2DArray_t& clusters,
					    const ::cv::Mat& img,
					    ImageMeta& meta,
					    ROI& roi)
  {
    if(this->ID()==0) throw larbys("DefectCluster should not be run 1st!");
    auto& data          = AlgoData<data::DefectClusterData>(0);
    auto& plane_data    = data._raw_cluster_vv[meta.plane()];

    // process vertex associated particle clusters if provided, and if not yet processed
    if( _vertextrack_algo_id!=kINVALID_ALGO_ID && data.get_vertex_clusters().empty()) {

      auto const& vtxtrack_data = AlgoData<data::VertexClusterArray>(_vertextrack_algo_id,0);

      // particle vertex cluster array
      const auto& vtx_cluster_v = vtxtrack_data._vtx_cluster_v;

      // loop over vtx
      for(auto const& vtx_cluster : vtx_cluster_v) {
	data::ParticleCompoundArray pcompound_set;
	// loop over plane
	for(size_t plane = 0 ; plane < vtx_cluster.num_planes(); ++plane) {
	  // loop over clusters on this plane
	  for(auto const& pcluster : vtx_cluster.get_clusters(plane)) {
	    LAROCV_INFO() << "Inspecting defects for Vertex " << vtx_cluster.id()
			  << " plane " << plane
			  << " particle " << pcluster.id()
			  << std::endl;
	    auto pcompound = _DefectBreaker.BreakContour(pcluster._ctor);
	    pcompound_set.emplace_back(plane,std::move(pcompound));
	  }
	}
	// record
	data.move(vtx_cluster.id(),std::move(pcompound_set));
      }
      LAROCV_INFO() << "Finished processing all vertex (result size = "
		    << data.num_vertex_clusters() << " vertex clusters)" << std::endl;
    }
    else if( _lineartrack_algo_id!=kINVALID_ALGO_ID && data.get_vertex_clusters().empty()) {

      auto const& lintrack_data = AlgoData<data::LinearTrackArray>(_lineartrack_algo_id,0);

      // particle vertex cluster array
      const auto& lin_cluster_v = lintrack_data.get_clusters();

      // loop over vtx
      for(auto const& lin_cluster : lin_cluster_v) {
	data::ParticleCompoundArray pcompound_set;
	// loop over plane
	for(size_t plane = 0 ; plane < 3; ++plane) {
	  // loop over clusters on this plane
	  auto& lcluster = lin_cluster.get_cluster(plane);
	  if ( lcluster.ctor.empty() ) continue;
	  LAROCV_INFO() << "Inspecting defects for LinearTrack " << lin_cluster.id()
			<< " plane " << plane
			<< std::endl;
	  auto pcompound = _DefectBreaker.BreakContour(lcluster.ctor);
	  pcompound_set.emplace_back(plane,std::move(pcompound));
	}
	// record
	data.move(lin_cluster.id(),std::move(pcompound_set));
      }
      LAROCV_INFO() << "Finished processing all vertex (result size = "
		    << data.num_vertex_clusters() << " vertex clusters)" << std::endl;


    }else{

      // Process input clusters on this plane
      for(size_t cindex=0; cindex<clusters.size(); ++cindex) {
	auto const& cluster = clusters[cindex];

	LAROCV_INFO() << "Inspecting defects plane " << meta.plane()
		      << " cluster " << cindex
		      << std::endl;

	auto cluscomp = _DefectBreaker.BreakContour(cluster._contour);

	plane_data.emplace_back(std::move(cluscomp));

      }
    }

    // Construct output
    Cluster2DArray_t oclusters_v;
    for(auto& cluscomp : plane_data.get_cluster()) {
      for(auto& atomic : cluscomp.get_atoms()) {
	Cluster2D ocluster;
	ocluster._contour = atomic;
	oclusters_v.emplace_back(std::move(ocluster));
      }
    }
    return oclusters_v;
  }


}
#endif
