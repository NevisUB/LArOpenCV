#ifndef __VERTEXSINGLESHOWER_CXX__
#define __VERTEXSINGLESHOWER_CXX__

#include "VertexSingleShower.h"

//#include <typeinfo>

namespace larocv {

  /// Global larocv::VertexSingleShowerFactory to register AlgoFactory
  static VertexSingleShowerFactory __global_VertexSingleShowerFactory__;

  void VertexSingleShower::Reset()
  { _algo.Reset(); _algo_seed.Reset(); }

  void VertexSingleShower::_Configure_(const Config_t &pset)
  {
    _algo.set_verbosity(this->logger().level());
    _algo.Configure(pset.get<Config_t>("OneTrackOneShower"));

    _algo_seed.set_verbosity(this->logger().level());
    _algo_seed.Configure(pset.get<Config_t>("ElectronShowerVertexSeed"));
    
    auto algo_name_dqdx  = pset.get<std::string>("dQdXProfilerName");
    auto algo_name_vertex_track = pset.get<std::string>("VertexTrackClusterName");
    auto algo_name_linear_track = pset.get<std::string>("LinearTrackClusterName");

    _algo_id_dqdx = this->ID( algo_name_dqdx );
    _algo_id_vertex_track = this->ID( algo_name_vertex_track );
    _algo_id_linear_track = this->ID( algo_name_linear_track );

    Register(new data::SingleShowerArray);
  }
  
  void VertexSingleShower::_Process_(const larocv::Cluster2DArray_t& clusters,
				     const ::cv::Mat& img,
				     larocv::ImageMeta& meta,
				     larocv::ROI& roi)
  {
    _algo.SetPlaneInfo(meta);
    _algo_seed.SetPlaneInfo(meta);
    return;
  }

  bool VertexSingleShower::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    // get data needed for preparing seed vertex information
    auto const& dqdx_data = AlgoData<data::dQdXProfilerData>     ( _algo_id_dqdx,         0);
    auto const& vtrack_data = AlgoData<data::VertexClusterArray> ( _algo_id_vertex_track, 0);
    auto const& ltrack_data = AlgoData<data::LinearTrackArray>   ( _algo_id_linear_track, 0);
    _algo_seed.RegisterSeed(ltrack_data);
    _algo_seed.RegisterSeed(img_v, vtrack_data, dqdx_data);

    // create seed vertex and register for one-shower-one-track algorithm
    _algo.RegisterSeed(_algo_seed.CreateSeed());

    auto shower_v = _algo.CreateSingleShower(img_v);
    
    auto& data = AlgoData<data::SingleShowerArray>(0);

    for(size_t i=0; i<shower_v.size(); ++i) 
      data.insert(i, std::move(shower_v[i]));

    // done
    return true;
  }

}
#endif
