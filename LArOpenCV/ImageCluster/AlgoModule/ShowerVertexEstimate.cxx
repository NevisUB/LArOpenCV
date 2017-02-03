#ifndef __SHOWERVERTEXESTIMATE_CXX__
#define __SHOWERVERTEXESTIMATE_CXX__

#include "ShowerVertexEstimate.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrackClusterData.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexClusterData.h"

namespace larocv {

  /// Global larocv::ShowerVertexEstimateFactory to register AlgoFactory
  static ShowerVertexEstimateFactory __global_ShowerVertexEstimateFactory__;

  void ShowerVertexEstimate::Reset()
  {  _algo_seed.Reset(); }
  
  void ShowerVertexEstimate::_Configure_(const Config_t &pset) {
    _algo_seed.set_verbosity(this->logger().level());
    _algo_seed.Configure(pset.get<Config_t>("ElectronShowerVertexSeed"));
    
    auto algo_name_vertex_track = pset.get<std::string>("VertexTrackClusterName");
    auto algo_name_linear_track = pset.get<std::string>("LinearTrackClusterName");

    _algo_id_vertex_track = kINVALID_ALGO_ID;
    _algo_id_linear_track = kINVALID_ALGO_ID;

    if (!algo_name_vertex_track.empty())
      _algo_id_vertex_track = this->ID( algo_name_vertex_track );

    if (!algo_name_linear_track.empty())
    _algo_id_linear_track = this->ID( algo_name_linear_track );
    
    Register(new data::ShowerVertexEstimateData);
  }

  void ShowerVertexEstimate::_Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi)
  {
    _algo_seed.SetPlaneInfo(meta);
    return;
  }


  bool ShowerVertexEstimate::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {

    if ( _algo_id_linear_track != kINVALID_ALGO_ID ) {
      auto const& ltrack_data = AlgoData<data::LinearTrackArray>(_algo_id_linear_track, 0);
      _algo_seed.RegisterSeed(ltrack_data);
    }

    if ( _algo_id_vertex_track != kINVALID_ALGO_ID ) {
      auto const& vtrack_data = AlgoData<data::VertexClusterArray>(_algo_id_vertex_track, 0);
      _algo_seed.RegisterSeed(img_v, vtrack_data);
    }

    auto vtx3d_v = _algo_seed.CreateSeed();

    LAROCV_DEBUG() << "Identified " << vtx3d_v.size() << " vertex candidates" << std::endl;
    
    
    return true;
  }
  
}
#endif

