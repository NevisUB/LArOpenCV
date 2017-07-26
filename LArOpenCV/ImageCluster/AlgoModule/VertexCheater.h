#ifndef __VERTEXCHEATER_H__
#define __VERTEXCHEATER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"


/*
  @brief: An VertexCheater
*/
namespace larocv {
 
  class VertexCheater : public larocv::ImageAnaBase {
    
  public:
    
    VertexCheater(const std::string name = "VertexCheater") :
      ImageAnaBase(name),
      _geo()
    {}
    
    virtual ~VertexCheater(){}
    
    void Finalize(TFile*) {}

    void Reset() {}

    void SetTrueVertex(const data::Vertex3D& true_vertex);
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const { return true; }

  private:

    LArPlaneGeo _geo;

    data::Vertex3D _true_vertex;

    float _default_radius;
  };

  class VertexCheaterFactory : public AlgoFactoryBase {
  public:
    VertexCheaterFactory() { AlgoFactory::get().add_factory("VertexCheater",this); }
    ~VertexCheaterFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new VertexCheater(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

