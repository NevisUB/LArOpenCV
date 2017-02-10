#ifndef __SHOWERVERTEXSEEDS_CXX__
#define __SHOWERVERTEXSEEDS_CXX__

#include "ShowerVertexSeeds.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
namespace larocv {

  /// Global ShowerVertexSeedsFactory to register AlgoFactory
  static ShowerVertexSeedsFactory __global_ShowerVertexSeedsFactory__;

  void ShowerVertexSeeds::_Configure_(const Config_t &pset)
  {
    _ElectronShowerVertexSeed.set_verbosity(this->logger().level());
    _ElectronShowerVertexSeed.Configure(pset.get<Config_t>("ElectronShowerVertexSeed"));

    auto name_vertex3d      = pset.get<std::string>("Vertex3DName");
    auto name_super_cluster = pset.get<std::string>("SuperClusterName");
    auto name_linear_track  = pset.get<std::string>("LiearTrackName");
    auto name_compound      = pset.get<std::string>("TrackParticleName");

    _vertex3d_id      = this->ID(name_vertex3d);
    _super_cluster_id = this->ID(name_super_cluster);
    _linear_track_id  = this->ID(name_linear_track);
    _compound_id      = this->ID(name_compound);

    Register(new data::VertexSeed3DArray);
  }

  bool ShowerVertexSeeds::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    auto const& input_vtx3d_v = AlgoData<data::Vertex3DArray>(_vertex3d_id,0);
    auto const& input_linear_track_v = AlgoData<data::LinearTrack3DArray>(_linear_track_id,0);
    
    std::vector<const data::ParticleClusterArray*> super_cluster_v;
    std::vector<const data::ParticleClusterArray*> part_cluster_v;
    std::vector<const data::TrackClusterCompoundArray*> compound_v;
    
    for(size_t plane=0; plane<img_v.size(); ++plane) {
      super_cluster_v.push_back(&(AlgoData<data::ParticleClusterArray>(_super_cluster_id,plane)));
      part_cluster_v.push_back(&(AlgoData<data::ParticleClusterArray>(_compound_id,plane)));
      compound_v.push_back(&(AlgoData<data::TrackClusterCompoundArray>(_compound_id,plane+img_v.size())));
    }

    _ElectronShowerVertexSeed.Reset();

    auto input_vtxinfo_v = data::OrganizeVertexInfo(AssManager(),input_vtx3d_v,super_cluster_v,part_cluster_v,compound_v);
    _ElectronShowerVertexSeed.RegisterSeed(input_linear_track_v);
    _ElectronShowerVertexSeed.RegisterSeed(img_v,input_vtxinfo_v);

    auto vtx3d_seed_v = _ElectronShowerVertexSeed.CreateSeed();
    
    auto& data = AlgoData<data::VertexSeed3DArray>(0);
    for(size_t i=0; i<vtx3d_seed_v.size(); ++i)
      data.emplace_back(std::move(vtx3d_seed_v[i]));

    return true;
  }
  
}
#endif
