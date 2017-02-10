#ifndef __SHOWERVERTEXESTIMATE_CXX__
#define __SHOWERVERTEXESTIMATE_CXX__

#include "ShowerVertexEstimate.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"

namespace larocv {

  /// Global larocv::ShowerVertexEstimateFactory to register AlgoFactory
  static ShowerVertexEstimateFactory __global_ShowerVertexEstimateFactory__;

  void ShowerVertexEstimate::Reset()
  {  _OneTrackOneShower.Reset(); }
  
  void ShowerVertexEstimate::_Configure_(const Config_t &pset) {
    _OneTrackOneShower.set_verbosity(this->logger().level());
    _OneTrackOneShower.Configure(pset.get<Config_t>("OneTrackOneShower"));
    
    auto algo_name_vertex_seed = pset.get<std::string>("ShowerVertexSeed");
    _algo_id_vertex_seed = this->ID( algo_name_vertex_seed );
    
    Register(new data::Vertex3DArray);
  }

  void ShowerVertexEstimate::_Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi)
  {
    _OneTrackOneShower.SetPlaneInfo(meta);
    return;
  }


  bool ShowerVertexEstimate::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    auto const& seed_v = AlgoData<data::VertexSeed3DArray>(_algo_id_vertex_seed,0);

    auto& data = AlgoData<data::Vertex3DArray>(0);

    _OneTrackOneShower.RegisterSeed(seed_v.as_vector());

    auto vtx3d_v = _OneTrackOneShower.CreateSingleShower(img_v);

    for(size_t i=0; i<vtx3d_v.size(); ++i)
      data.emplace_back(std::move(vtx3d_v[i]));
    
    return true;
  }
  
}
#endif

