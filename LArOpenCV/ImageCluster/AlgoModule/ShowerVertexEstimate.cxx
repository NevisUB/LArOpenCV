#ifndef __SHOWERVERTEXESTIMATE_CXX__
#define __SHOWERVERTEXESTIMATE_CXX__

#include "ShowerVertexEstimate.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"

namespace larocv {

  /// Global larocv::ShowerVertexEstimateFactory to register AlgoFactory
  static ShowerVertexEstimateFactory __global_ShowerVertexEstimateFactory__;

  void ShowerVertexEstimate::Reset()
  {  _OneTrackOneShower.Reset(); }
  
  void ShowerVertexEstimate::_Configure_(const Config_t &pset) {
    _OneTrackOneShower.set_verbosity(this->logger().level());
    _OneTrackOneShower.Configure(pset.get<Config_t>("OneTrackOneShower"));
    
    auto algo_name_vertex_seed = pset.get<std::string>("ShowerVertexSeed","");
    _algo_id_vertex_seed = this->ID( algo_name_vertex_seed );
    if (!algo_name_vertex_seed.empty()) {
      _algo_id_vertex_seed = this->ID( algo_name_vertex_seed );
      if(_algo_id_vertex_seed==kINVALID_ALGO_ID)
	throw larbys("You specified an invalid ShowerVertexSeed algorithm name");
    }

    auto algo_name_shower_from_track_vertex = pset.get<std::string>("ShowerOnTrackEnd","");
    _algo_id_shower_track_vertex=kINVALID_ALGO_ID;
    if (!algo_name_shower_from_track_vertex.empty()) {
      _algo_id_shower_track_vertex = this->ID( algo_name_shower_from_track_vertex );
      if(_algo_id_shower_track_vertex==kINVALID_ALGO_ID)
	throw larbys("You specified an invalid ShowerFromTrackVertex algorithm name");
    }

    auto algo_name_track_vertex_estimate = pset.get<std::string>("TrackVertexEstimate","");
    _algo_id_track_vertex_estimate=kINVALID_ALGO_ID;
    if (!algo_name_track_vertex_estimate.empty()) {
      _algo_id_track_vertex_estimate = this->ID( algo_name_track_vertex_estimate );
      if(_algo_id_track_vertex_estimate==kINVALID_ALGO_ID)
	throw larbys("You specified an invalid TrackVertexEstimate algorithm name");
    }

    auto algo_name_track_vertex_particle_cluster = pset.get<std::string>("TrackVertexParticleCluster","");
    _algo_id_track_vertex_particle_cluster=kINVALID_ALGO_ID;
    if (!algo_name_track_vertex_particle_cluster.empty()) {
      _algo_id_track_vertex_particle_cluster = this->ID( algo_name_track_vertex_particle_cluster );
      if(_algo_id_track_vertex_particle_cluster==kINVALID_ALGO_ID)
	throw larbys("You specified an invalid TrackVertexParticleCluster algorithm name");
    }

    
    Register(new data::Vertex3DArray);
  }

  void ShowerVertexEstimate::_Process_(const larocv::Cluster2DArray_t& clusters,
				       ::cv::Mat& img,
				       larocv::ImageMeta& meta,
				       larocv::ROI& roi)
  {
    _OneTrackOneShower.SetPlaneInfo(meta);
    return;
  }


  bool ShowerVertexEstimate::_PostProcess_(std::vector<cv::Mat>& img_v)
  {

    auto const& seed_v = AlgoData<data::VertexSeed3DArray>(_algo_id_vertex_seed,0);
    
    auto& data = AlgoData<data::Vertex3DArray>(0);

    _OneTrackOneShower.RegisterSeed(seed_v.as_vector());

    auto vtx3d_v = _OneTrackOneShower.CreateSingleShower(img_v);
    
    LAROCV_DEBUG() << "Found " << vtx3d_v.size() << " single shower vertex" << std::endl;
    for(size_t i=0; i<vtx3d_v.size(); ++i) {
      data.emplace_back(std::move(vtx3d_v[i]));
    }

    if (_algo_id_shower_track_vertex!=kINVALID_ALGO_ID and _algo_id_track_vertex_estimate != kINVALID_ALGO_ID) {
      auto& ass_man = AssManager();

      const auto& track_vertex_v = AlgoData<data::Vertex3DArray>(_algo_id_track_vertex_estimate,0);
      const auto& shower_track_v = AlgoData<data::Vertex3DArray>(_algo_id_shower_track_vertex,0);
      
      for(const auto& vtx3d :  shower_track_v.as_vector())  {
	data::Vertex3D vtx3d_f = vtx3d;
	
	auto ass_id = ass_man.GetOneAss(vtx3d,track_vertex_v.ID());
	if (ass_id==kINVALID_SIZE)
	  throw larbys("Associated track vertex cannot be found, sorry");
	
	data.emplace_back(std::move(vtx3d_f));
	AssociateOne(track_vertex_v.as_vector()[ass_id],data.as_vector().back());

	for(size_t plane=0;plane<3;++plane) {
	  const auto& track_par_v = AlgoData<data::ParticleClusterArray>(_algo_id_track_vertex_particle_cluster,plane);
	  auto par_ass_id = ass_man.GetOneAss(vtx3d,track_par_v.ID());
	  if (par_ass_id==kINVALID_SIZE) continue;
	  AssociateOne(data.as_vector().back(),track_par_v.as_vector()[par_ass_id]);
	}
      }
    }    
    
    return true;
  }
  
  
}
#endif

