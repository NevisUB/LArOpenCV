#ifndef __LINEARITYANALYSIS_H__
#define __LINEARITYANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: XXX
*/
namespace larocv {
 
  class LinearityAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    LinearityAnalysis(const std::string name = "LinearityAnalysis") :
      ImageAnaBase(name),
      _tree(nullptr)
    {}
    
    virtual ~LinearityAnalysis(){}
    
    void Finalize(TFile* fout)
    { fout->cd(); _tree->Write(); }

  protected:
    
    void Reset() { }

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const { return true; }
    
  private:
    
    AlgorithmID_t _combined_id;
    AlgorithmID_t _particle_id;

    TTree* _tree;
    
    int _roid;
    int _vtxid;

    double _x;
    double _y;
    double _z;
    
  };

  class LinearityAnalysisFactory : public AlgoFactoryBase {
  public:
    LinearityAnalysisFactory() { AlgoFactory::get().add_factory("LinearityAnalysis",this); }
    ~LinearityAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new LinearityAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

