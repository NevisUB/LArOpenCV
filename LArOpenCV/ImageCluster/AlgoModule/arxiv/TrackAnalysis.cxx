#ifndef __TRACKANALYSIS_CXX__
#define __TRACKANALYSIS_CXX__

#include "TrackAnalysis.h"

namespace larocv {

  /// Global TrackAnalysisFactory to register AlgoFactory
  static TrackAnalysisFactory __global_TrackAnalysisFactory__;

  void TrackAnalysis::_Configure_(const Config_t &pset)
  {
    _SuperClusterer.set_verbosity(this->logger().level());
    _SuperClusterer.Configure(pset.get<Config_t>("SuperClusterer"));
    Register(new data::ContourArrayData); // plane 0
    Register(new data::ContourArrayData); // plane 1
    Register(new data::ContourArrayData); // plane 2
  }

  void TrackAnalysis::_Process_(const Cluster2DArray_t& clusters,
				const ::cv::Mat& img,
				ImageMeta& meta,
				ROI& roi)
  {
    auto& data = AlgoData<data::ContourArrayData>(meta.plane());

    _SuperClusterer.CreateSuperCluster(img,data);



// // get out vertex_data from ProcessFunction
//     auto& vtx_data = AlgoData<data::VertexClusterArray>(0);
    
//     // particle vertex cluster array
//     auto& vtx_cluster_v = vtx_data._vtx_cluster_v;
    
//     // loop over vtx
//     for(auto& pclusarray : vtx_cluster_v) {
      
//       // loop over planes
//       for(size_t plane = 0 ; plane < pclusarray.num_planes(); ++plane) {

// 	auto circle_vtx_c = pclusarray.get_circle_vertex(plane).center;
// 	LAROCV_DEBUG() << "Scanning 2D vertex @ " << circle_vtx_c << " on plane " << plane << std::endl;
	
// 	// loop over vertex clusters on this plane --> each one is particle cluster
// 	for(auto& pcluster : pclusarray.get_clusters_writeable(plane)) {
// 	  LAROCV_INFO() << "Inspecting defects for Vertex " << pclusarray.id()
// 			<< " plane " << plane
// 			<< " particle " << pcluster.id()
// 			<< std::endl;

// 	  // break this particle cluster
// 	  auto pcompound = _DefectBreaker.BreakContour(pcluster._ctor);

//     	  // order atomics in this particle cluster
// 	  auto const ordered_atom_id_v = _AtomicAnalysis.OrderAtoms(pcompound,circle_vtx_c);

// 	  if(this->logger().level() == msg::kDEBUG) {
// 	    std::stringstream ss;
// 	    ss << "Setting order [";
// 	    for(auto idx : ordered_atom_id_v)
// 	      ss << idx << ",";
// 	    ss << "]";
// 	    LAROCV_DEBUG() << ss.str() << std::endl;
// 	  }
	  
// 	  // set the ordering
// 	  pcompound.set_atomic_order(ordered_atom_id_v);
	  
// 	  // get start/end points for this atomic
// 	  auto atom_edges_v = _AtomicAnalysis.AtomsEdge(pcompound, circle_vtx_c, ordered_atom_id_v);

// 	  LAROCV_DEBUG() << "Found " << pcompound.size() << " atomic(s)!" << std::endl;
	  
// 	  // set per atomic information
// 	  for (size_t atom_id=0; atom_id<pcompound.size(); ++atom_id) {
// 	    auto& atomic = pcompound[atom_id];
// 	    LAROCV_DEBUG() << "... id " << atomic.id() << " of size " << atomic.size() << " at index " << atom_id << std::endl;

// 	    // set the edges
// 	    auto& start_end = atom_edges_v[atom_id];
// 	    atomic.add_edge(start_end.first);
// 	    atomic.add_edge(start_end.second);
	    
// 	    // calculate the PCA per atomic
// 	    atomic.set_pca(CalcPCA(atomic));
	    
// 	    // calculate the dQdX per atomic
// 	    std::vector<float> atom_dqdx;
// 	    try { 
// 	      atom_dqdx = _AtomicAnalysis.AtomdQdX(img_v[plane],atomic,atomic.pca(),start_end.first, start_end.second);
// 	    } catch (const larbys& err) {
// 	      LAROCV_NORMAL() << "AtomdQdX could not be discerned" << std::endl;
// 	      atom_dqdx = {};
// 	    }
// 	    LAROCV_DEBUG() << "Calculated dQdX of length " << atom_dqdx.size() << std::endl;

// 	    atomic.set_dqdx(atom_dqdx);
	    
// 	    // refine the last atomic end point
// 	    if (atomic.id() == ordered_atom_id_v.back()) {
// 	      LAROCV_DEBUG() << "Refining this atomic end point" << std::endl;
// 	      _AtomicAnalysis.RefineAtomicEndPoint(img_v[plane],atomic);
// 	      pcompound.set_end_pt(atomic.edges()[1]);
// 	    }
// 	  }


// 	  pcluster = pcompound;
// 	}
// 	LAROCV_DEBUG() << "End clusters on plane " << plane << std::endl;
//       }
//     }
   
    
  }
  

}
#endif
