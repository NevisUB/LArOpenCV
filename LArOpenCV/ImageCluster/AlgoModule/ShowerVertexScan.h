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
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi)
    {
      _geo.ResetPlaneInfo(meta);
      _vtxana.ResetPlaneInfo(meta);
      _VertexScan3D.SetPlaneInfo(meta);
    }

    
    bool _PostProcess_(std::vector<cv::Mat>& img_v);

  private:
    float _threshold;
    AlgorithmID_t _seed_id;

    LArPlaneGeo _geo;
    VertexAnalysis _vtxana;
    VertexScan3D  _VertexScan3D;
    
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

