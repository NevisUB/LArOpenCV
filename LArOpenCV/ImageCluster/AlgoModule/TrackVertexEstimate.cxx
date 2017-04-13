#ifndef __TRACKVERTEXESTIMATE_CXX__
#define __TRACKVERTEXESTIMATE_CXX__

#include "TrackVertexEstimate.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

namespace larocv {

  /// Global larocv::TrackVertexEstimateFactory to register AlgoFactory
  static TrackVertexEstimateFactory __global_TrackVertexEstimateFactory__;

  void TrackVertexEstimate::_Configure_(const Config_t &pset) {
    // Set algo verboity
    _algo.set_verbosity(this->logger().level());
    // Set algo configure
    _algo.Configure(pset.get<Config_t>("TrackVertexScan2D"));
    
    // Get the id index of module --- TrackVertexSeedsAlgoName
    _vertex_seed_algo_id = kINVALID_ID;

    auto const vertex_seed_algo_name = pset.get<std::string>("TrackVertexSeedsAlgoName","");
    if(!vertex_seed_algo_name.empty()) _vertex_seed_algo_id = this->ID(vertex_seed_algo_name);

    // Create a container for this algorithm data
    Register(new data::Vertex3DArray);
  }

  bool TrackVertexEstimate::_PostProcess_() const
  { return true; }
  
  void TrackVertexEstimate::_Process_()
  {
    // Read the track images (configured in cfg file)
    auto img_v = ImageArray();
    auto const& meta_v = MetaArray();

    for(size_t img_idx=0; img_idx<img_v.size(); ++img_idx) {

      auto& img = img_v[img_idx];
      auto const& meta = meta_v.at(img_idx);
      // Read in VertexSeeds
      const auto& vertex_seeds_v = AlgoData<data::VertexSeed2DArray>(_vertex_seed_algo_id,meta.plane());
      std::vector<geo2d::Vector<float> > seeds_v;    
      for(const auto& seed : vertex_seeds_v.as_vector())
	seeds_v.emplace_back(seed);
      
      LAROCV_DEBUG() << "Scanning " << seeds_v.size() << " seeds on plane " << meta.plane() << std::endl;

      //Scan points in seeds_v and find the local minimum of two angles made by
      //local pcas
      //v.s.
      //circle center and xs points 

      _algo.AnalyzePlane(img,meta,seeds_v);
    }

    std::vector<data::Vertex3D> vtx3d_v;
    std::vector<std::vector<data::CircleVertex> > vtx2d_vv;
    
    //
    //0) Find 2D Vertices from time scanning
    //
    _algo.CreateTimeVertex3D(img_v,vtx3d_v,vtx2d_vv);
    
    auto& vertex3d_v  = AlgoData<data::Vertex3DArray>(0);
    
    for(uint idx=0;idx<vtx3d_v.size();++idx) {
      auto& vtx2d_v=vtx2d_vv[idx];
      auto& vtx3d=vtx3d_v[idx];
      vtx3d.type=data::VertexType_t::kTime;
      vtx3d.cvtx2d_v=std::move(vtx2d_v);
      vertex3d_v.emplace_back(std::move(vtx3d));
    }
    
    //
    //1) Find 2D Vertices from wire scanning
    //
    vtx3d_v.clear();
    vtx2d_vv.clear();
    _algo.CreateWireVertex3D(img_v,vtx3d_v,vtx2d_vv);

    for(uint idx=0;idx<vtx3d_v.size();++idx) {
      auto& vtx2d_v=vtx2d_vv[idx];
      auto& vtx3d=vtx3d_v[idx];
      vtx3d.type=data::VertexType_t::kWire;
      vtx3d.cvtx2d_v=std::move(vtx2d_v);
      vertex3d_v.emplace_back(std::move(vtx3d));
    }

    LAROCV_DEBUG() << "Inferred " << vertex3d_v.as_vector().size() << std::endl;
    // Print out the vertex index for fun(I lied, for debugging).
    for(const auto& vtx :  vertex3d_v.as_vector())
      LAROCV_DEBUG() << "... id " << vtx.ID() << std::endl;
    
    Reset();
  }
  
}
#endif

