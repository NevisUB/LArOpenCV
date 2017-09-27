#ifndef __REANAVERTEX_CXX__
#define __REANAVERTEX_CXX__

#include "ReAnaVertex.h"
#include "LArOpenCV/ImageCluster/AlgoData/ParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv {
  
  /// Global larocv::ReAnaVertexFactory to register AlgoFactory
  static ReAnaVertexFactory __global_ReAnaVertexFactory__;

  void ReAnaVertex::Clear()
  {
    _vertex_v.clear();
    _ctor_vvv.clear();
  }

  void ReAnaVertex::_Configure_(const Config_t &pset) {
    _nplanes = 3;

    _ReCreateVertex.Configure(pset.get<Config_t>("ReCreateVertex"));
    
    Register(new data::Vertex3DArray);
    for(size_t plane=0;plane<_nplanes;++plane) Register(new data::ParticleClusterArray);
  }
  
  void ReAnaVertex::_Process_()
  {
    LAROCV_INFO() << "start" << std::endl;

    for(auto const& meta : MetaArray()) 
      _ReCreateVertex.SetPlaneInfo(meta);
    
    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto thresh_img_v = adc_img_v;
    for(auto& img : thresh_img_v)
      img = Threshold(img,10,255);

    auto& vertex_data = AlgoData<data::Vertex3DArray>(0);
    
    LAROCV_DEBUG() << "GOT: "<< _vertex_v.size() << " vertices"  << std::endl;
    for(size_t vid=0; vid<_vertex_v.size(); ++vid) {
      auto& vertex = _vertex_v[vid];
      vertex.vtx2d_v.resize(3);
      vertex.cvtx2d_v.resize(3);   
      
      _ReCreateVertex.ReCreate(vertex,thresh_img_v);
      
      
      //
      // ...hack for now...
      //
      bool bad = false;
      auto& ctor_vv = _ctor_vvv[vid];
      for(size_t plane=0; plane < 3; ++plane) {
	const auto& ctor_v   = ctor_vv[plane];
	for(const auto& ctor : ctor_v) {
	  if (ctor.empty()) continue;
	  if (vertex.cvtx2d_v.at(plane).radius == -1) {
	    bad = true;
	  }
	}
      }
      if(bad) continue;

      vertex_data.emplace_back(std::move(vertex));
      const auto& vtx3d = vertex_data.as_vector().back();

      for(size_t plane=0; plane < 3; ++plane) {
	LAROCV_DEBUG() << "set particles" << std::endl;
	auto& par_data = AlgoData<data::ParticleClusterArray>(plane+1);
	auto& ctor_v   = ctor_vv[plane];
	
	LAROCV_DEBUG() << "@plane=" << plane << " with " << ctor_v.size() << " contours" << std::endl;
	for(auto& ctor : ctor_v) {
	  if (ctor.empty()) continue;
	  if (vtx3d.cvtx2d_v.at(plane).radius == -1) {
	    LAROCV_CRITICAL() << "d=" << Pt2PtDistance(vtx3d.cvtx2d_v.at(plane).center,ctor) << std::endl;
	    auto thresh = thresh_img_v[plane].clone();
	    cv::drawContours(thresh,std::vector<GEO2D_Contour_t>(1,ctor),-1,cv::Scalar(150));
	    cv::imwrite("out.png",thresh);
	    LAROCV_CRITICAL() << "x=" << vtx3d.cvtx2d_v.at(plane).center.x << " y=" << vtx3d.cvtx2d_v.at(plane).center.y << std::endl;
	    throw larbys();
	  }
	  LAROCV_DEBUG() << "... inserting sz " << ctor.size() << std::endl;
	  data::ParticleCluster par;
	  par._ctor = std::move(ctor);
	  par.type  = data::ParticleType_t::kUnknown;
	  par_data.emplace_back(std::move(par));
	  AssociateMany(vtx3d,par_data.as_vector().back());
	} // end particle
	LAROCV_DEBUG() << "...next plane" << std::endl;
      } // end plane
      LAROCV_DEBUG() << "...next vertex" << std::endl;
    } // end vertex
      
    LAROCV_INFO() << "end" << std::endl;
    Clear();
  }
  
  bool ReAnaVertex::_PostProcess_() const
  { return true; }
   
}
#endif

