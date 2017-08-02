#ifndef __VERTEXCHEATER_CXX__
#define __VERTEXCHEATER_CXX__

#include "VertexCheater.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {

  /// Global larocv::VertexCheaterFactory to register AlgoFactory
  static VertexCheaterFactory __global_VertexCheaterFactory__;
  
  void VertexCheater::_Configure_(const Config_t &pset)
  {

    _default_radius = pset.get<float>("DefaultRadius",6);
    
    LAROCV_DEBUG() << "Register data::Vertex3DArray" << std::endl;
    Register(new data::Vertex3DArray);
  }

  void VertexCheater::_Process_()
  {

    for(auto const& meta : MetaArray())
      _geo.ResetPlaneInfo(meta);

    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto thresh_img_v = adc_img_v;
    for(auto& img : thresh_img_v)
      img = Threshold(img,10,255);
    
    auto& cheater_data = AlgoData<data::Vertex3DArray>(0);

    _true_vertex.vtx2d_v.resize(3);
    _true_vertex.cvtx2d_v.resize(3);
    
    //
    // Set 2D information
    //
    for(size_t plane=0; plane<3; ++plane) {
      LAROCV_DEBUG() << "@ plane " << plane << " setting... " << std::endl;

      auto& vtx2d  = _true_vertex.vtx2d_v[plane];
      auto& cvtx2d = _true_vertex.cvtx2d_v[plane];


      cvtx2d.radius = _default_radius;
      
      float x = kINVALID_FLOAT;
      try {
	x = _geo.x2col (_true_vertex.x,plane);
      } catch(const larbys& what) {
	LAROCV_WARNING() << "Projecting X [cm] " << _true_vertex.x << " on to plane " << plane << " OUT OF RANGE" << std::endl;
	cvtx2d.radius = -1.0;
      }
      
      float y = kINVALID_FLOAT;
      try {
	y = _geo.yz2row(_true_vertex.y,_true_vertex.z,plane);
      } catch(const larbys& what) {
	LAROCV_WARNING() << "Projecting Y [cm] " << _true_vertex.y << " Z [cm] " << _true_vertex.z << " on to plane " << plane << " OUT OF RANGE" << std::endl;
	cvtx2d.radius = -1.0;
      }

      LAROCV_DEBUG() << " @ vertex " << &_true_vertex << " (x,y,z)=("<<_true_vertex.x<<","<<_true_vertex.y<<","<<_true_vertex.z<<")"<<std::endl;
      LAROCV_DEBUG() << " => (x,y)=(col,row)=("<<x<<","<<y<<")"<<std::endl;
	
      vtx2d.pt.x = x;
      vtx2d.pt.y = y;
	
      cvtx2d.center.x = x;
      cvtx2d.center.y = y;
      
      const auto& thresh_img = thresh_img_v.at(plane);
      auto& xs_v = cvtx2d.xs_v;
      auto circle = cvtx2d.as_circle();
	
      auto xs_pt_v = QPointOnCircle(thresh_img,circle);
      xs_v.reserve(xs_pt_v.size());

      for(const auto& xs_pt : xs_pt_v) {
	geo2d::Line<float> local_pca;
	local_pca.pt = cv::Point_<float>(kINVALID_FLOAT,kINVALID_FLOAT);
	local_pca.dir = local_pca.pt;
	try {
	  local_pca   = SquarePCA(thresh_img, xs_pt, 2,2);
	} catch (const larbys& what) {}

	xs_v.push_back(data::PointPCA(xs_pt, local_pca));
      }
      
    }

    cheater_data.emplace_back(std::move(_true_vertex));
    
    LAROCV_DEBUG() << "return " << cheater_data.as_vector().size() << " true vertex"  << std::endl;
    
    return;
  }
  
  void VertexCheater::SetTrueVertex(const data::Vertex3D& true_vertex) {
    LAROCV_DEBUG() << "Set true vertex @ (x,y,z)="
		   << "(" << true_vertex.x << "," << true_vertex.y << "," << true_vertex.z << ")" << std::endl;
    
    _true_vertex = true_vertex;

    return;
  }

}
#endif
