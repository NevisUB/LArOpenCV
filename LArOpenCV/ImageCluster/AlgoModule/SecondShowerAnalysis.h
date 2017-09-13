#ifndef __SECONDSHOWERANALYSIS_H__
#define __SECONDSHOWERANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PixelScan3D.h"

/*
  @brief: XXX
*/
namespace larocv {
 
  class SecondShowerAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    SecondShowerAnalysis(const std::string name = "SecondShowerAnalysis") :
      ImageAnaBase(name),
      _tree(nullptr)
    {}
    
    virtual ~SecondShowerAnalysis(){}
    
    void Finalize(TFile* fout)
    { fout->cd(); _tree->Write(); }

  protected:
    
    void Reset() { }

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const { return true; }
    
  private:
    TTree* _tree;

    int _roid;
    int _vtxid;

    double _x;
    double _y;
    double _z;

    AlgorithmID_t _particle_id;
    AlgorithmID_t _combined_id;

    void ClearVertex();
    void ResetVectors(size_t sz);

    
  public:
    PixelScan3D _PixelScan3D;
    
  };

  class SecondShowerAnalysisFactory : public AlgoFactoryBase {
  public:
    SecondShowerAnalysisFactory() { AlgoFactory::get().add_factory("SecondShowerAnalysis",this); }
    ~SecondShowerAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new SecondShowerAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

