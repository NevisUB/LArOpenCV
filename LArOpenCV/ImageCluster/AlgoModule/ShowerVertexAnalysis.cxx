#ifndef __SHOWERVERTEXANALYSIS_CXX__
#define __SHOWERVERTEXANALYSIS_CXX__

#include "ShowerVertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {

  /// Global larocv::ShowerVertexAnalysisFactory to register AlgoFactory
  static ShowerVertexAnalysisFactory __global_ShowerVertexAnalysisFactory__;

  void ShowerVertexAnalysis::Reset()
  {}
  
  void ShowerVertexAnalysis::_Configure_(const Config_t &pset) {

    _track_vertex_algo_id = kINVALID_ALGO_ID;
    _shower_vertex_algo_id = kINVALID_ALGO_ID;    
    
    auto track_vertex_algo_name = pset.get<std::string>("TrackVertexEstimate","");
    if (!track_vertex_algo_name.empty()) {
      _track_vertex_algo_id = this->ID(track_vertex_algo_name);
      if (_track_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given TrackVertexEstimate name is INVALID!");
    }

    auto shower_vertex_algo_name = pset.get<std::string>("ShowerVertexEstimate","");
    if (!shower_vertex_algo_name.empty()) {
      _shower_vertex_algo_id = this->ID(shower_vertex_algo_name);
      if (_shower_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerVertexEstimate name is INVALID!");
    }
    
    
    Register(new data::Vertex3DArray);
  }
  
  
  bool ShowerVertexAnalysis::_PostProcess_(const std::vector<const cv::Mat>& img_v)
  {
    LAROCV_DEBUG() << "start" << std::endl;

    //track vertex
    const auto& track_vtx_data = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id,0);
    //shower vertex
    const auto& shower_vtx_data = AlgoData<data::Vertex3DArray>(_shower_vertex_algo_id,0);

    auto& ass_man = AssManager();
    
    auto& vtx_data = AlgoData<data::Vertex3DArray>(0);
    for(const auto& shower_vtx : shower_vtx_data.as_vector()) {
      if (shower_vtx.type==3) {
	auto ass_id = ass_man.GetOneAss(shower_vtx,track_vtx_data.ID());
	LAROCV_DEBUG() << "Found associated ass id " << ass_id << " for shower vertex @ " << &shower_vtx << std::endl;

	if (ass_id==kINVALID_SIZE)
	  throw larbys("Could not determine associated track-shower vertex");
	
	LAROCV_DEBUG() << "Inserting associated track vertex" << std::endl;
	const auto& track_vertex = track_vtx_data.as_vector()[ass_id];
	vtx_data.push_back(track_vertex);	
      }
      else vtx_data.push_back(shower_vtx);
    }
    

    LAROCV_DEBUG() << "end" << std::endl;
    return true;
  }
   
}
#endif

