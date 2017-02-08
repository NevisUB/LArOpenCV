#ifndef __SHOWERPARTICLECLUSTER_CXX__
#define __SHOWERPARTICLECLUSTER_CXX__

#include "ShowerParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexEstimateData.h"

namespace larocv {

  /// Global larocv::ShowerParticleClusterFactory to register AlgoFactory
  static ShowerParticleClusterFactory __global_ShowerParticleClusterFactory__;
  
  void ShowerParticleCluster::_Configure_(const Config_t &pset) {

    _algo.set_verbosity(this->logger().level());
    _algo.Configure(pset.get<Config_t>("OneTrackOneShower"));
    
    auto algo_name_shower_estimate = pset.get<std::string>("ShowerVertexEstimateName");
    _algo_id_shower_estimate = kINVALID_ALGO_ID;
    if(!algo_name_shower_estimate.empty())
      _algo_id_shower_estimate = this->ID( algo_name_shower_estimate );

    auto algo_name_shower_super_cluster = pset.get<std::string>("ShowerSuperClusterName");
    _algo_id_shower_super_cluster = kINVALID_ALGO_ID;
    if(!algo_name_shower_super_cluster.empty())
      _algo_id_shower_super_cluster = this->ID( algo_name_shower_super_cluster );
	
    Register(new data::SingleShowerArray);
  }

  void ShowerParticleCluster::_Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta,
				       larocv::ROI& roi)
  {
    _algo.SetPlaneInfo(meta);    
    return;
  }
  
  
  bool ShowerParticleCluster::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    const auto& vertex_seed_data = AlgoData<data::VertexEstimateData>(_algo_id_shower_estimate,0);
    const auto& seeds = vertex_seed_data.get_vertex();
      
    _algo.RegisterSeed(seeds);
 
    auto shower_v = _algo.CreateSingleShower(img_v);
    
    auto& data = AlgoData<data::SingleShowerArray>(0);
    
    for(size_t i=0; i<shower_v.size(); ++i) 
      data.insert(i, std::move(shower_v[i]));
    
    return true;
  }
  
}
#endif

