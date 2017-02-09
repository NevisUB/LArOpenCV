#ifndef __TRACKVERTEXESTIMATE_CXX__
#define __TRACKVERTEXESTIMATE_CXX__

#include "TrackVertexEstimate.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

namespace larocv {

  /// Global larocv::TrackVertexEstimateFactory to register AlgoFactory
  static TrackVertexEstimateFactory __global_TrackVertexEstimateFactory__;

  void TrackVertexEstimate::_Configure_(const Config_t &pset) {

    _algo.set_verbosity(this->logger().level());
    _algo.Configure(pset.get<Config_t>("TrackVertexScan2D"));

    _vertex_seed_algo_id = kINVALID_ID;

    auto const vertex_seed_algo_name = pset.get<std::string>("TrackVertexSeedsAlgoName","");
    if(!vertex_seed_algo_name.empty()) _vertex_seed_algo_id = this->ID(vertex_seed_algo_name);

    Register(new data::Vertex3DArray);
  }

  void TrackVertexEstimate::_Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi)
  {

    const auto& vertex_seeds_v = AlgoData<data::VertexSeed2DArray>(_vertex_seed_algo_id,meta.plane());
    std::vector<geo2d::Vector<float> > seeds_v;    
    for(const auto& seed : vertex_seeds_v.as_vector())
      seeds_v.emplace_back(seed);
    
    LAROCV_DEBUG() << "Scanning " << seeds_v.size() << " seeds on plane " << meta.plane() << std::endl;
    _algo.AnalyzePlane(img,meta,seeds_v); //vertex_seeds_v.as_vector() --> why this failed here?
    
    return;
  }


  bool TrackVertexEstimate::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    std::vector<data::Vertex3D> vtx3d_v;
    std::vector<std::vector<data::CircleVertex> > vtx2d_vv;
    _algo.CreateTimeVertex3D(img_v,vtx3d_v,vtx2d_vv);

    auto& vertex3d_v  = AlgoData<data::Vertex3DArray>(0);

    for(uint idx=0;idx<vtx3d_v.size();++idx) {
      auto& vtx2d_v=vtx2d_vv[idx];
      auto& vtx3d=vtx3d_v[idx];
      vtx3d.type=0;
      vtx3d.cvtx2d_v=std::move(vtx2d_v);
      vertex3d_v.emplace_back(std::move(vtx3d));
    }


    vtx3d_v.clear();
    vtx2d_vv.clear();
    _algo.CreateWireVertex3D(img_v,vtx3d_v,vtx2d_vv);

    for(uint idx=0;idx<vtx3d_v.size();++idx) {
      auto& vtx2d_v=vtx2d_vv[idx];
      auto& vtx3d=vtx3d_v[idx];
      vtx3d.type=1;
      vtx3d.cvtx2d_v=std::move(vtx2d_v);
      vertex3d_v.emplace_back(std::move(vtx3d));
    }

    Reset();
    return true;
  }
  
}
#endif

