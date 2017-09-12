#ifndef __REANAVERTEX_H__
#define __REANAVERTEX_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
#include "LArOpenCV/ImageCluster/AlgoData/Vertex.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexScan3D.h"

namespace larocv {

  class ReAnaVertex : public larocv::ImageAnaBase {

  public:
  ReAnaVertex(const std::string name = "ReAnaVertex") :
    ImageAnaBase(name) {}
    ~ReAnaVertex(){}
    void Reset() {}
    void Finalize(TFile*) {}

    std::vector<data::Vertex3D> _vertex_v;
    std::vector<std::array<GEO2D_ContourArray_t, 3> > _ctor_vvv;

  protected:
    void _Configure_(const Config_t &pset);
    void _Process_();
    bool _PostProcess_() const;

  private:
    void Clear();

    LArPlaneGeo _geo;
    size_t _nplanes;
    float _default_radius;
    bool _scan_radius;
    VertexScan3D _VertexScan3D;
  };
  
  class ReAnaVertexFactory : public AlgoFactoryBase {
  public:
    ReAnaVertexFactory() { AlgoFactory::get().add_factory("ReAnaVertex",this); }
    ~ReAnaVertexFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new ReAnaVertex(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

