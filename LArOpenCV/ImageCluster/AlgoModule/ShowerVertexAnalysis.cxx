#ifndef __SHOWERVERTEXANALYSIS_CXX__
#define __SHOWERVERTEXANALYSIS_CXX__

#include "ShowerVertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {

  /// Global larocv::ShowerVertexAnalysisFactory to register AlgoFactory
  static ShowerVertexAnalysisFactory __global_ShowerVertexAnalysisFactory__;

  void ShowerVertexAnalysis::Reset()
  {}
  
  void ShowerVertexAnalysis::_Configure_(const Config_t &pset) {

    
    _merge  = pset.get<bool>("Merge");
    _input_adc_img = pset.get<bool>("InputIsADCImage");
      
    auto shower_vertex_algo_name = pset.get<std::string>("ShowerVertex","");
    if (!shower_vertex_algo_name.empty()) {
      _shower_vertex_algo_id = this->ID(shower_vertex_algo_name);
      if (_shower_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerVertex name is INVALID!");
    }

    auto track_vertex_algo_name = pset.get<std::string>("TrackVertex","");
    if (!track_vertex_algo_name.empty()) {
      _track_vertex_algo_id = this->ID(track_vertex_algo_name);
      if (_track_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given TrackVertex name is INVALID!");
    }

    auto scluster_algo_name = pset.get<std::string>("ShowerSuperClusterAlgo");
    if (!scluster_algo_name.empty()) {
      _scluster_algo_id = this->ID(scluster_algo_name);
      if (_scluster_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerSuperClusterAlgo name is INVALID!");
    }

    auto acluster_algo_name = pset.get<std::string>("ADCSuperClusterAlgo");
    if (!acluster_algo_name.empty()) {
      _acluster_algo_id = this->ID(acluster_algo_name);
      if (_acluster_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ADCSuperClusterAlgo name is INVALID!");
    }
    
    
    _ClusterMerge.Configure(pset.get<Config_t>("ClusterMerge"));
    
    Register(new data::Vertex3DArray);
    for(size_t planeid=0;planeid<3;++planeid)
      Register(new data::ParticleClusterArray);
    for(size_t planeid=0;planeid<3;++planeid)
      Register(new data::TrackClusterCompoundArray);
    
  }
  
  bool ShowerVertexAnalysis::_PostProcess_(std::vector<cv::Mat>& img_v)
  {
    auto& ass_man = AssManager();
    auto& vertex_data = AlgoData<data::Vertex3DArray>(0);
    const auto& shower_vertex_data = AlgoData<data::Vertex3DArray>(_shower_vertex_algo_id,0);
    const auto& track_vertex_data  = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id,0);    
    std::vector<GEO2D_ContourArray_t> super_ctor_vv;
    std::vector<GEO2D_ContourArray_t> adc_ctor_vv;
    super_ctor_vv.resize(3);
    adc_ctor_vv.resize(3);
    for(size_t plane=0;plane<3;++plane) {
      const auto& super_cluster_v = AlgoData<data::ParticleClusterArray>(_scluster_algo_id,plane).as_vector();
      const auto& adc_cluster_v = AlgoData<data::ParticleClusterArray>(_acluster_algo_id,plane).as_vector();
      auto& super_ctor_v=super_ctor_vv[plane];
      auto& adc_ctor_v=adc_ctor_vv[plane];
      super_ctor_v.reserve(super_cluster_v.size());
      adc_ctor_v.reserve(adc_cluster_v.size());
      for(const auto& super_cluster : super_cluster_v)
	super_ctor_v.emplace_back(super_cluster._ctor);
      for(const auto& adc_cluster : adc_cluster_v)
	adc_ctor_v.emplace_back(adc_cluster._ctor);
    }
    
    for(const auto& vtx3d : shower_vertex_data.as_vector()) {
      vertex_data.push_back(vtx3d);
      auto& vtx3d_copy = vertex_data.as_vector().back();
      if (vtx3d.type==data::VertexType_t::kEndOfTrack) {
	auto ass_vtx_id = ass_man.GetOneAss(vtx3d,track_vertex_data.ID());
	if (ass_vtx_id==kINVALID_SIZE) {
	  LAROCV_CRITICAL()<<"Unassociated end-of-track vertex"<<std::endl;
	  throw larbys();
	}
	AssociateOne(vtx3d_copy,track_vertex_data.as_vector()[ass_vtx_id]);
      }
      
      for(size_t plane=0;plane<3;++plane) {
	const auto& super_ctor_v = super_ctor_vv[plane];
	const auto& adc_ctor_v = adc_ctor_vv[plane];
	
	auto& par_data = AlgoData<data::ParticleClusterArray>(plane+1);
	const auto& shower_par_data = AlgoData<data::ParticleClusterArray>(_shower_vertex_algo_id,plane+1);

	auto& comp_data = AlgoData<data::TrackClusterCompoundArray>(3+plane+1);
	const auto& shower_comp_data = AlgoData<data::TrackClusterCompoundArray>(_shower_vertex_algo_id,3+plane+1);

	auto shower_par_ass_id_v = ass_man.GetManyAss(vtx3d,shower_par_data.ID());	
	for(auto shower_par_id : shower_par_ass_id_v) {
	  const auto& par = shower_par_data.as_vector()[shower_par_id];
	  if (par.type==data::ParticleType_t::kShower) {
	    auto merged_par = par;
	    if (_merge) {

	      GEO2D_Contour_t merged_ctor;
	      if (_input_adc_img)
		merged_ctor = _ClusterMerge.FlashlightMerge(vtx3d.vtx2d_v[plane].pt,super_ctor_v,par._ctor);
	      else
		merged_ctor = _ClusterMerge.FlashlightMerge(vtx3d.vtx2d_v[plane].pt,
							    super_ctor_v,
							    adc_ctor_v,
							    par._ctor);
	      
	      merged_ctor = FindNonZero(MaskImage(img_v[plane],merged_ctor,3,false)); // add some padding
	      merged_ctor = MergeByMask(merged_ctor,par._ctor,BlankImage(img_v[plane]));
	      merged_ctor = ConvexHull(merged_ctor);
	      merged_par._ctor=std::move(merged_ctor);
	      LAROCV_DEBUG() << "Merged shower contour of size " << par._ctor.size()
			     << " into size " << merged_par._ctor.size() << std::endl;
	      if(this->logger().level() == ::larocv::msg::kDEBUG) {
		auto origin_pts = CountNonZero(MaskImage(img_v[plane],par._ctor,0,false));
		auto merged_pts = CountNonZero(MaskImage(img_v[plane],merged_par._ctor,3,false));
		LAROCV_DEBUG() << "... made " << origin_pts << " ==> " << merged_pts << std::endl;
	      }
	    }
	    par_data.emplace_back(std::move(merged_par));
	    AssociateMany(vtx3d_copy,par_data.as_vector().back());
	  }
	  else if (par.type==data::ParticleType_t::kTrack) {
	    par_data.push_back(par);
	    auto track_comp_id = ass_man.GetOneAss(par,shower_comp_data.ID());
	    if (track_comp_id==kINVALID_SIZE) throw larbys("Invalid comp id requested");
	    const auto& track_comp = shower_comp_data.as_vector()[track_comp_id];
	    AssociateMany(vtx3d_copy,par_data.as_vector().back());
	    comp_data.push_back(track_comp);
	    AssociateOne(par_data.as_vector().back(),comp_data.as_vector().back());
	  }
	}	
      }
    }
    
    return true;
  }
   
}
#endif

