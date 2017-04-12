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
    //
    // Prepare algorithms via config
    //

    // Algorithm for determine valid 3D shower vertex seeds
    _ElectronShowerVertexSeed.set_verbosity(this->logger().level());
    _ElectronShowerVertexSeed.Configure(pset.get<Config_t>("ElectronShowerVertexSeed"));

    // Track vertex seed data input
    auto name_vertex3d      = pset.get<std::string>("Vertex3DName","");
    auto name_super_cluster = pset.get<std::string>("SuperClusterName");
    auto name_compound      = pset.get<std::string>("TrackParticleName","");

    // Shower vertex seed data input
    auto name_linear_track  = pset.get<std::string>("LinearTrackName");

    if (!name_vertex3d.empty()) {
      _vertex3d_id      = this->ID(name_vertex3d);
      if (_vertex3d_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "Vertex3D algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }

    if(!name_compound.empty()) {
      _compound_id      = this->ID(name_compound);
      if (_compound_id == kINVALID_ALGO_ID) {
	LAROCV_CRITICAL() << "TrackParticle algorithm name does not exist!" << std::endl;
	throw larbys();
      }
    }
    
    _super_cluster_id = this->ID(name_super_cluster);
    _linear_track_id  = this->ID(name_linear_track);


    // Create a container for this algorithm data
    Register(new data::VertexSeed3DArray);
  }

  bool ShowerVertexSeeds::_PostProcess_() const
  { return true; }
  
  void ShowerVertexSeeds::_Process_()
  {
    //
    // 0) Prep
    //
    
    auto img_v = ImageArray();
    for(auto const& meta : MetaArray())
      _ElectronShowerVertexSeed.SetPlaneInfo(meta);

    // Set up per plane arrays of super cluster, particle cluster, and compound
    std::vector<const data::ParticleClusterArray*> super_cluster_v;
    std::vector<const data::ParticleClusterArray*> part_cluster_v;
    std::vector<const data::TrackClusterCompoundArray*> comp_cluster_v;
    
    for(size_t plane=0; plane<img_v.size(); ++plane) {
      super_cluster_v.push_back(&(AlgoData<data::ParticleClusterArray>(_super_cluster_id,plane)));
      if (_compound_id!=kINVALID_ALGO_ID) {
	part_cluster_v.push_back(&(AlgoData<data::ParticleClusterArray>(_compound_id,plane)));
	comp_cluster_v.push_back(&(AlgoData<data::TrackClusterCompoundArray>(_compound_id,plane+img_v.size())));
      }
    }

    //
    // 1) Organize algorithm data information to estimate 3D shower vertex
    //

    // Get 3D seeds from track verticies
    std::vector<data::VertexTrackInfoCollection> input_vtxinfo_v;
    if (_vertex3d_id!=kINVALID_ALGO_ID) {
      auto const& input_vtx3d_v = AlgoData<data::Vertex3DArray>(_vertex3d_id,0);
      
      input_vtxinfo_v = data::OrganizeVertexInfo(AssManager(),
						 input_vtx3d_v,
						 super_cluster_v,
						 part_cluster_v,
						 comp_cluster_v);
    }
    
    LAROCV_INFO() << "input_vtxinfo_v size " << input_vtxinfo_v.size() << std::endl;

    // Get 3D seeds from linear tracks
    auto const& input_linear_track_v = AlgoData<data::LinearTrack3DArray>(_linear_track_id,0);

    // Register both seeds to seed estimation algorithm
    _ElectronShowerVertexSeed.RegisterSeed(img_v,input_vtxinfo_v);
    _ElectronShowerVertexSeed.RegisterSeed(input_linear_track_v);

    // Return valid seeds
    auto vtx3d_seed_v = _ElectronShowerVertexSeed.CreateSeed();

    // Write 3D seeds to the output
    auto& data = AlgoData<data::VertexSeed3DArray>(0);
    for(size_t i=0; i<vtx3d_seed_v.size(); ++i)
      data.emplace_back(std::move(vtx3d_seed_v[i]));
    
  }
}
#endif
