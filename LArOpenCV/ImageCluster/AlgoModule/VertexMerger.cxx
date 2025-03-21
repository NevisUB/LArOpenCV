#ifndef __VERTEXMERGER_CXX__
#define __VERTEXMERGER_CXX__

#include "VertexMerger.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {

  /// Global larocv::VertexMergerFactory to register AlgoFactory
  static VertexMergerFactory __global_VertexMergerFactory__;
  
  void VertexMerger::_Configure_(const Config_t &pset)
  {

    auto track_vertex_algo_name = pset.get<std::string>("TrackVertexAlgo","");
    _track_vertex_algo_id = kINVALID_ALGO_ID;
    if (!track_vertex_algo_name.empty()) {
      _track_vertex_algo_id = this->ID(track_vertex_algo_name);
      if (_track_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given TrackVertex name is INVALID!");
    }
    
    auto shower_vertex_algo_name = pset.get<std::string>("ShowerVertexAlgo","");
    _shower_vertex_algo_id = kINVALID_ALGO_ID;
    if (!shower_vertex_algo_name.empty()) {
      _shower_vertex_algo_id = this->ID(shower_vertex_algo_name);
      if (_shower_vertex_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ShowerVertex name is INVALID!");
    }

    auto adc_super_algo_name = pset.get<std::string>("ADCSuperAlgo","");
    _adc_super_algo_id = kINVALID_ALGO_ID;
    if (!adc_super_algo_name.empty()) {
      _adc_super_algo_id = this->ID(adc_super_algo_name);
      if (_adc_super_algo_id==kINVALID_ALGO_ID)
	throw larbys("Given ADCSuper name is INVALID!");
    }

    
  }

  bool VertexMerger::_PostProcess_() const
  { return true; }
  
  void VertexMerger::_Process_() {

    auto& assman = AssManager();
    
    const auto& trk_vtx_data = AlgoData<data::Vertex3DArray>(_track_vertex_algo_id ,0);
    const auto& shr_vtx_data = AlgoData<data::Vertex3DArray>(_shower_vertex_algo_id,0);

    const auto& trk_vtx_v = trk_vtx_data.as_vector();
    const auto& shr_vtx_v = shr_vtx_data.as_vector();

    auto num_trk_vtx = trk_vtx_v.size();
    auto num_shr_vtx = shr_vtx_v.size();
      
    auto num_vtx = num_shr_vtx + num_trk_vtx;
    std::vector<const data::Vertex3D*> vtx3d_ptr_v;
    vtx3d_ptr_v.reserve(num_vtx);

    for(const auto& vtx : trk_vtx_v) vtx3d_ptr_v.emplace_back(&vtx);
    for(const auto& vtx : shr_vtx_v) vtx3d_ptr_v.emplace_back(&vtx);

    std::vector<VertexMeta> vtx3d_meta_v;
    vtx3d_meta_v.resize(num_vtx);

    for(size_t vtx3d_id=0; vtx3d_id < num_vtx; ++vtx3d_id) {

      const auto& vtx3d = *(vtx3d_ptr_v[vtx3d_id]);
      auto& vtx3d_meta = vtx3d_meta_v[vtx3d_id];
      
      auto& valid_plane_v    = vtx3d_meta.valid_plane_v;
      auto& weight_plane_v   = vtx3d_meta.weight_plane_v;
      auto& ass_vtx2d_adc_v  = vtx3d_meta.ass_vtx2d_adc_v;
      auto& ass_cvtx2d_adc_v = vtx3d_meta.ass_cvtx2d_adc_v;
      auto& avg_weight       = vtx3d_meta.avg_weight;
      auto& nweight_planes   = vtx3d_meta.nweight_planes;
	
      for(size_t plane=0; plane<3; ++plane) {
	const auto& vtx2d = vtx3d.vtx2d_v[plane];
	const auto& cvtx  = vtx3d.cvtx2d_v[plane];
	
	const auto& adc_super_data = AlgoData<data::ParticleClusterArray>(_adc_super_algo_id,plane);
	const auto& adc_super_v = adc_super_data.as_vector();
	GEO2D_ContourArray_t adc_ctor_v;
	adc_ctor_v.reserve(adc_super_v.size());
	for(const auto& par : adc_super_v) adc_ctor_v.push_back(par._ctor);

	auto adc_vtx2d_ass_id = FindContainingContour(adc_ctor_v,vtx2d.pt);
	ass_vtx2d_adc_v[plane] = adc_vtx2d_ass_id;
	
	if (cvtx.weight<0) continue;
	valid_plane_v[plane] = true;
	weight_plane_v[plane] = cvtx.weight;
	
	avg_weight += cvtx.weight;
	nweight_planes++;
	
	auto adc_cvtx2d_ass_id = assman.GetOneAss(vtx3d,adc_super_data.ID());
	if (adc_cvtx2d_ass_id == kINVALID_SIZE) continue;
	ass_cvtx2d_adc_v[plane] = adc_cvtx2d_ass_id;

	
	auto center_line0 = geo2d::Line<float>(cvtx.xs_v[0].pt, cvtx.xs_v[0].pt - cvtx.center);
	auto center_line1 = geo2d::Line<float>(cvtx.xs_v[1].pt, cvtx.xs_v[1].pt - cvtx.center);
	auto dtheta = fabs(geo2d::angle(center_line0) - geo2d::angle(center_line1));
	LAROCV_DEBUG() << "dtheta " << dtheta << " nxs " << cvtx.xs_v.size() << " rad " << cvtx.radius << std::endl;
      }
      
      avg_weight /= (float)nweight_planes;

      LAROCV_DEBUG() << "id " << vtx3d_id << "):" << std::endl;
      LAROCV_DEBUG() << vtx3d_meta.dump() << std::endl;
      
    } // end vertex
    
    
  }
}
#endif
