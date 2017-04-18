#ifndef __TRACKVERTEXSCAN_H__
#define __TRACKVERTEXSCAN_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexScan3D.h"

/*
  @brief: XYZ
*/
namespace larocv {
 
  class TrackVertexScan : public larocv::ImageAnaBase {
    
  public:
    
    TrackVertexScan(const std::string name = "TrackVertexScan") :
      ImageAnaBase(name),
      _geo(),
      _vtxana(),
      _VertexScan3D()
    {}
    
    virtual ~TrackVertexScan(){}
    
    void Finalize(TFile*) {}

    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:
    AlgorithmID_t _seed_id;
    AlgorithmID_t _vertex_estimate_algo_id;
    
    LArPlaneGeo _geo;
    VertexAnalysis _vtxana;
    VertexScan3D  _VertexScan3D;

    float _allowed_radius;
    float _merge_distance;
    bool _require_3planes_charge;
    bool _merge_nearby;

    
    
    
  };

  class TrackVertexScanFactory : public AlgoFactoryBase {
  public:
    TrackVertexScanFactory() { AlgoFactory::get().add_factory("TrackVertexScan",this); }
    ~TrackVertexScanFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new TrackVertexScan(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

