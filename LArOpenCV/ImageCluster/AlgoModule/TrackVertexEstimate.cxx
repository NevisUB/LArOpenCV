#ifndef __TRACKVERTEXESTIMATE_CXX__
#define __TRACKVERTEXESTIMATE_CXX__

#include "TrackVertexEstimate.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeedsData.h"
#include "LArOpenCV/ImageCluster/AlgoData/Refine2DVertexData.h"

namespace larocv {

  /// Global larocv::TrackVertexEstimateFactory to register AlgoFactory
  static TrackVertexEstimateFactory __global_TrackVertexEstimateFactory__;

  void TrackVertexEstimate::_Configure_(const Config_t &pset) {

    _algo.set_verbosity(this->logger().level());
    _algo.Configure(pset.get<Config_t>("TrackVertexScan2D"));

    _vertex_seed_algo_id = kINVALID_ID;

    auto const vertex_seed_algo_name = pset.get<std::string>("VertexSeedsAlgoName","");
    if(!vertex_seed_algo_name.empty()) _vertex_seed_algo_id = this->ID(vertex_seed_algo_name);
  
    Register(new data::TrackVertexEstimateData);
  }

  void TrackVertexEstimate::_Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi)
  {

    const auto& vertex_seeds_data = AlgoData<data::VertexSeedsData>(_vertex_seed_algo_id,0);

    auto points = vertex_seeds_data.harvest_seed_points(meta.plane());
    
    _algo.AnalyzePlane(img,meta,points);
    
    return;
  }


  bool TrackVertexEstimate::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    std::vector<data::Vertex3D> vtx3d_v;
    std::vector<std::vector<data::CircleVertex> > vtx2d_vv;
    _algo.CreateTimeVertex3D(img_v,vtx3d_v,vtx2d_vv);

    auto& data = AlgoData<data::Refine2DVertexData>(0);

    for(size_t idx=0; idx<vtx3d_v.size(); ++idx)
      data.emplace_back(0,std::move(vtx3d_v[idx]),std::move(vtx2d_vv[idx]));

    vtx3d_v.clear();
    vtx2d_vv.clear();
    _algo.CreateWireVertex3D(img_v,vtx3d_v,vtx2d_vv);

    for(size_t idx=0; idx<vtx3d_v.size(); ++idx)
      data.emplace_back(0,std::move(vtx3d_v[idx]),std::move(vtx2d_vv[idx]));

    return false;
  }
  
}
#endif
