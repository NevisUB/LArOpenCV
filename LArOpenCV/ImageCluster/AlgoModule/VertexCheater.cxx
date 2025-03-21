#ifndef __VERTEXCHEATER_CXX__
#define __VERTEXCHEATER_CXX__

#include "VertexCheater.h"
#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

namespace larocv {

  /// Global larocv::VertexCheaterFactory to register AlgoFactory
  static VertexCheaterFactory __global_VertexCheaterFactory__;
  
  void VertexCheater::_Configure_(const Config_t &pset)
  {
    _ReCreateVertex.Configure(pset.get<Config_t>("ReCreateVertex"));
    LAROCV_DEBUG() << "Register data::Vertex3DArray" << std::endl;
    Register(new data::Vertex3DArray);
  }

  void VertexCheater::_Process_()
  {
    for(auto const& meta : MetaArray())
      _ReCreateVertex.SetPlaneInfo(meta);

    auto adc_img_v = ImageArray(ImageSetID_t::kImageSetWire);
    auto thresh_img_v = adc_img_v;
    for(auto& img : thresh_img_v)
      img = Threshold(img,10,255);
    
    auto& cheater_data = AlgoData<data::Vertex3DArray>(0);

    _true_vertex.vtx2d_v.resize(3);
    _true_vertex.cvtx2d_v.resize(3);
   
    _ReCreateVertex.ReCreate(_true_vertex,thresh_img_v);

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
