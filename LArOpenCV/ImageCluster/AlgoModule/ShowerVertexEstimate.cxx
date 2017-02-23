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

    auto algo_name_shower_from_track_vertex = pset.get<std::string>("ShowerOnTrackEnd","");
    _algo_id_shower_track_vertex=kINVALID_ALGO_ID;
    if (!algo_name_shower_from_track_vertex.empty()) {
      _algo_id_shower_track_vertex = this->ID( algo_name_shower_from_track_vertex );
      if(_algo_id_shower_track_vertex==kINVALID_ALGO_ID)
	throw larbys("You specified an invalid ShowerFromTrackVertex algorithm name");
    }
    
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
    
    LAROCV_DEBUG() << "Found " << vtx3d_v.size() << " single shower vertex" << std::endl;
    for(size_t i=0; i<vtx3d_v.size(); ++i)
      data.emplace_back(std::move(vtx3d_v[i]));

    if (_algo_id_shower_track_vertex!=kINVALID_ALGO_ID) {
      const auto& shower_track_v = AlgoData<data::Vertex3DArray>(_algo_id_shower_track_vertex,0);
      LAROCV_DEBUG() << "Found " << shower_track_v.as_vector().size() << " edge vertex" << std::endl;
      for(const auto& vtx3d :  shower_track_v.as_vector()) 
	data.push_back(vtx3d);
    }    
    
    return true;
  }
  
}
#endif

