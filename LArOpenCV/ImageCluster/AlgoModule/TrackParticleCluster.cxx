#ifndef __TRACKPARTICLECLUSTER_CXX__
#define __TRACKPARTICLECLUSTER_CXX__

#include "TrackVertexEstimate.h"
#include "TrackParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.cxx"

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

    _contour_pad = pset.get<float>("ContourPad",0.);

    Register(new data::VertexClusterArray);
  }

  void TrackParticleCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta,
				       larocv::ROI& roi)
  {
    // Algorithm data
    auto& vtx_data = AlgoData<data::VertexClusterArray>(0);

    // if given from Refine2DVertex, fill
    if(_track_vertex_estimate_algo_id!=kINVALID_ID && vtx_data._vtx_cluster_v.empty()) {
      
      auto const& ref_data = AlgoData<data::VertexEstimateData>(_track_vertex_estimate_algo_id,0);

      vtx_data._vtx_cluster_v.resize(ref_data.get_vertex().size());
      
      for(size_t vtx_id = 0; vtx_id < ref_data.get_vertex().size(); ++vtx_id) {

	auto const& vtx3d = ref_data.get_vertex()[vtx_id];
	auto const& circle_vtx_v = ref_data.get_circle_vertex(vtx3d.id());
	
	auto& vtx_cluster = vtx_data._vtx_cluster_v[vtx_id];
	vtx_cluster.set_vertex(vtx3d, circle_vtx_v);
      }
    }

    // Run clustering for this plane & store
    auto const plane = meta.plane();
    
    for(size_t vtx_id = 0; vtx_id < vtx_data._vtx_cluster_v.size(); ++vtx_id) {
      
      auto& vtx_cluster = vtx_data._vtx_cluster_v[vtx_id];
      auto const& circle_vtx = vtx_cluster.get_circle_vertex(plane);
      LAROCV_DEBUG() << "Vertex ID " << vtx_id << " plane " << plane
		     << " CircleVertex @ " << circle_vtx.center << " w/ R = " << circle_vtx.radius << std::endl;
      
      // Create track contours from the vertex point
      auto contour_v = _VertexParticleCluster.CreateParticleCluster(img,circle_vtx);

      //
      // Analyze fraction of pixels clustered
      // 0) find a contour that contains the subject 2D vertex, and find allcontained non-zero pixels inside
      // 1) per track cluster count # of pixels from 0) that is contained inside

      auto const parent_ctor_id = _VertexParticleCluster.FindSuperCluster(circle_vtx.center);
      geo2d::VectorArray<int> parent_points;
      if(parent_ctor_id != kINVALID_SIZE) {
	auto const& parent_ctor = _VertexParticleCluster.SuperClusters()[parent_ctor_id];
	geo2d::VectorArray<int> points;
	findNonZero(img, points);
	parent_points.reserve(points.size());
	for(auto const& pt : points) {
	  auto dist = ::cv::pointPolygonTest(parent_ctor,pt,true);
	  if(dist<-1.0*_contour_pad) continue;
	  parent_points.push_back(pt);
	}
      }
      vtx_cluster.set_num_pixel(plane,parent_points.size());

      LAROCV_INFO() << "# non-zero pixels " << parent_points.size()
		    << " ... # track clusters " << contour_v.size()
		    << std::endl;
      
      for(size_t cidx=0; cidx<contour_v.size(); ++cidx) {
	auto& contour = contour_v[cidx];
	LAROCV_DEBUG() << "On contour: " << cidx << "... size: " << contour.size() << std::endl;
	data::ParticleCluster cluster;
	cluster._ctor = std::move(contour);
	// Find # pixels in this cluster from the parent
	size_t num_pixel = 0;
	float qsum = 0;
	  for(auto const& pt : parent_points) {
	    // LAROCV_DEBUG() << "pt: " << pt << std::endl;
	    auto dist = ::cv::pointPolygonTest(cluster._ctor,pt,true);
	    // LAROCV_DEBUG() << "... @ dist: " << dist << std::endl;
	    if(dist<-1.0*_contour_pad) continue;
	    num_pixel += 1;
	    qsum      += (float)img.at<uchar>(pt.y,pt.x);
	  }
	LAROCV_DEBUG() << "Inserting num_pixel: " << num_pixel << "... qsum: " << qsum << std::endl;
	cluster._num_pixel = num_pixel;
	cluster._qsum = qsum;
	vtx_cluster.emplace_back(plane,std::move(cluster));
      }
    }

    
    return;
  }
  
  
  bool TrackParticleCluster::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {

    // get out vertex_data from ProcessFunction
    auto& vtx_data = AlgoData<data::VertexClusterArray>(0);
    
    // particle vertex cluster array
    auto& vtx_cluster_v = vtx_data._vtx_cluster_v;
    
    // loop over vtx
    for(auto& pclusarray : vtx_cluster_v) {
      
      // loop over planes
      for(size_t plane = 0 ; plane < pclusarray.num_planes(); ++plane) {

	auto circle_vtx_c = pclusarray.get_circle_vertex(plane).center;
	LAROCV_DEBUG() << "Scanning 2D vertex @ " << circle_vtx_c << " on plane " << plane << std::endl;
	
	// loop over vertex clusters on this plane --> each one is particle cluster
	for(auto& pcluster : pclusarray.get_clusters_writeable(plane)) {
	  LAROCV_INFO() << "Inspecting defects for Vertex " << pclusarray.id()
			<< " plane " << plane
			<< " particle " << pcluster.id()
			<< std::endl;

	  // break this particle cluster
	  auto pcompound = _DefectBreaker.BreakContour(pcluster._ctor);

    	  // order atomics in this particle cluster
	  auto const ordered_atom_id_v = _AtomicAnalysis.OrderAtoms(pcompound,circle_vtx_c);
    
	  // get start/end points for this atomic
	  auto atom_edges_v = _AtomicAnalysis.AtomsEdge(pcompound, circle_vtx_c, ordered_atom_id_v);

	  LAROCV_DEBUG() << "Found " << pcompound.size() << " atomic(s)!" << std::endl;

	  // set per atomic information
	  for (size_t atom_id=0; atom_id<pcompound.size(); ++atom_id) {
	    auto& atomic = pcompound[atom_id];
	    LAROCV_DEBUG() << "... id " << atomic.id() << " of size " << atomic.size() << " at index " << atom_id << std::endl;

	    // set the edges
	    auto& start_end = atom_edges_v[atom_id];
	    atomic.add_edge(start_end.first);
	    atomic.add_edge(start_end.second);
	    
	    // calculate the PCA per atomic
	    atomic.set_pca(CalcPCA(atomic));
	    
	    // calculate the dQdX per atomic
	    std::vector<float> atom_dqdx;
	    try { 
	      atom_dqdx = _AtomicAnalysis.AtomdQdX(img_v[plane],atomic,atomic.pca(),start_end.first, start_end.second);
	    } catch (const larbys& err) {
	      LAROCV_NORMAL() << "AtomdQdX could not be discerned" << std::endl;
	      atom_dqdx = {};
	    }

	    atomic.set_dqdx(atom_dqdx);
	    
	    // refine the last atomic end point
	    if (atomic.id() == ordered_atom_id_v.back()) {
	      _AtomicAnalysis.RefineAtomicEndPoint(img_v[plane],atomic);
	      pcompound.set_end_pt(atomic.edges()[1]);
	    }
	    
	    pcluster = pcompound;
	  } 
	}
	LAROCV_DEBUG() << "End clusters on plane " << plane << std::endl;
      }
    }
    
    return true;
  } // end _PostProcess_
  
}
#endif

