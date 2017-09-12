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
    _default_radius = pset.get<float>("DefaultRadius",4.0);
    _scan_radius = pset.get<bool>("ScanRadius",false);
    _VertexScan3D.Configure(pset.get<Config_t>("VertexScan3D"));

    _nplanes = 3;
    Register(new data::Vertex3DArray);
    for(size_t plane=0;plane<_nplanes;++plane) Register(new data::ParticleClusterArray);
  }
  
  void ReAnaVertex::_Process_()
  {
    LAROCV_INFO() << "start" << std::endl;

    for(auto const& meta : MetaArray()) {
      _VertexScan3D.SetPlaneInfo(meta);
      _geo.ResetPlaneInfo(meta);
    }
    
    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto thresh_img_v = adc_img_v;
    for(auto& img : thresh_img_v) {
      img = Threshold(img,10,255);
    }

    auto& vertex_data = AlgoData<data::Vertex3DArray>(0);
    
    LAROCV_DEBUG() << "GOT: "<< _vertex_v.size() << " vertices"  << std::endl;
    for(size_t vid=0; vid<_vertex_v.size(); ++vid) {
      auto& vertex = _vertex_v[vid];
      vertex.vtx2d_v.resize(3);
      vertex.cvtx2d_v.resize(3);   
      
      auto& ctor_vv = _ctor_vvv[vid];

      LAROCV_DEBUG() << "@ vid=" <<vid << " got " << ctor_vv.size() << " planes" << std::endl;

      for(size_t plane=0; plane < 3; ++plane) {

	LAROCV_DEBUG() << "@ plane=" << plane << std::endl;
	//
	// Project the vertex to this plane
	//
	auto& vtx2d  = vertex.vtx2d_v[plane];
	auto& cvtx2d = vertex.cvtx2d_v[plane];

	float x = kINVALID_FLOAT;
	float y = kINVALID_FLOAT;

	try { x = _geo.x2col (vertex.x,plane); } 
	catch(const larbys& what) { continue;}
	
	try { y = _geo.yz2row(vertex.y,vertex.z,plane); }
	catch(const larbys& what) { continue; }
	
	LAROCV_DEBUG() << " @ vertex " << &vertex << " (x,y,z)=("<<vertex.x<<","<<vertex.y<<","<<vertex.z<<")"<<std::endl;
	LAROCV_DEBUG() << " => (x,y)=(col,row)=("<<x<<","<<y<<")"<<std::endl;
	
	vtx2d.pt.x = x;
	vtx2d.pt.y = y;
	
	//
	// Determine the appropriate radius
	//
	const auto& thresh_img = thresh_img_v[plane];
	if (_scan_radius) { 
	  geo2d::Vector<float> plane_pt(kINVALID_FLOAT,kINVALID_FLOAT);
	  auto seed = data::VertexSeed3D(vertex);
	  auto res = _VertexScan3D.SetPlanePoint(thresh_img,seed,plane,plane_pt);
	  LAROCV_DEBUG() << "res=" << res << " plane_pt=" << plane_pt << std::endl;
	  cvtx2d = _VertexScan3D.RadialScan2D(thresh_img,plane_pt); 
	  LAROCV_DEBUG() << "cvtx2d.radius=" << cvtx2d.radius << std::endl;
	  LAROCV_DEBUG() << "cvtx2d.xs_pts_v=" << cvtx2d.xs_v.size() << std::endl;
	}
	else { 
	  //
	  // Set QPoint
	  //
	  
	  cvtx2d.center.x = x;
	  cvtx2d.center.y = y;
	  cvtx2d.radius = _default_radius;

	  auto& xs_v = cvtx2d.xs_v;
	  auto circle = cvtx2d.as_circle();
	
	  auto xs_pt_v = QPointOnCircle(thresh_img,circle);
	  xs_v.reserve(xs_pt_v.size());

	  for(const auto& xs_pt : xs_pt_v) {
	    geo2d::Line<float> local_pca;
	    local_pca.pt = cv::Point_<float>(kINVALID_FLOAT,kINVALID_FLOAT);
	    local_pca.dir = local_pca.pt;
	    try { local_pca   = SquarePCA(thresh_img, xs_pt, 2,2); }
	    catch (const larbys& what) {}
	    xs_v.push_back(data::PointPCA(xs_pt, local_pca));
	  }
	}
      }

      vertex_data.emplace_back(std::move(vertex));
      const auto& vtx3d = vertex_data.as_vector().back();

      // std::stringstream ss;
      // ss << vid << "_";

      for(size_t plane=0; plane < 3; ++plane) {
	LAROCV_DEBUG() << "set particles" << std::endl;
	auto& par_data = AlgoData<data::ParticleClusterArray>(plane+1);
	auto& ctor_v   = ctor_vv[plane];

	// ss << plane << ".png";
	// auto plane_img = thresh_img_v.at(plane).clone();
	// cv::drawContours(plane_img,ctor_v,-1,cv::Scalar(150));
	// cv::imwrite(ss.str(),plane_img);
	// ss.str(std::string());
	// ss << vid << "_";
	
	LAROCV_DEBUG() << "@plane=" << plane << " with " << ctor_v.size() << " contours" << std::endl;
	for(auto& ctor : ctor_v) {
	  if (ctor.empty()) continue;
	  if ( vtx3d.cvtx2d_v.at(plane).radius == -1) {
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

