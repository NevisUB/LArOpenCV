#ifndef __SHOWERVERTEXSCAN_H__
#define __SHOWERVERTEXSCAN_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexScan3D.h"

/*
  @brief: XYZ
*/
namespace larocv {
 
  class ShowerVertexScan : public larocv::ImageAnaBase {
    
  public:
    
    ShowerVertexScan(const std::string name = "ShowerVertexScan") :
      ImageAnaBase(name),
      _geo(),
      _vtxana(),
      _VertexScan3D()
    {}
    
    virtual ~ShowerVertexScan(){}
    
    void Finalize(TFile*) {}

    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:
    AlgorithmID_t _seed_id;
    AlgorithmID_t _adc_super_cluster_algo;
    
    LArPlaneGeo _geo;
    VertexAnalysis _vtxana;
    VertexScan3D  _VertexScan3D;

    float _allowed_radius;
    float _merge_distance;
    bool _require_3planes_charge;
    bool _merge_nearby;


    
  };

  class ShowerVertexScanFactory : public AlgoFactoryBase {
  public:
    ShowerVertexScanFactory() { AlgoFactory::get().add_factory("ShowerVertexScan",this); }
    ~ShowerVertexScanFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new ShowerVertexScan(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

