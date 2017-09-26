#ifndef __RECREATEVERTEX_CXX__
#define __RECREATEVERTEX_CXX__

#include "ReCreateVertex.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

namespace larocv { 

  void ReCreateVertex::Configure(const Config_t& pset) {
    _default_radius = pset.get<float>("DefaultRadius",4.0);
    _scan_radius    = pset.get<bool>("ScanRadius",false);
    _VertexScan3D.Configure(pset.get<Config_t>("VertexScan3D"));
  }  

  void ReCreateVertex::SetPlaneInfo(const ImageMeta& meta) {
    _geo.ResetPlaneInfo(meta);
    _VertexScan3D.SetPlaneInfo(meta);
  }

  void ReCreateVertex::ReCreate(data::Vertex3D& vertex,const std::vector<cv::Mat>& thresh_img_v) {

    for(size_t plane=0; plane < 3; ++plane) {

      const auto& thresh_img =  thresh_img_v.at(plane);

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
	} // end xspt
      } // end set qpoint
    } // end plane
  } // end ReCreate(...)

}

#endif
