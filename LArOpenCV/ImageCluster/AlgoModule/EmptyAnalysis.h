#ifndef __EMPTYANALYSIS_H__
#define __EMPTYANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: XXX
*/
namespace larocv {
 
  class EmptyAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    EmptyAnalysis(const std::string name = "EmptyAnalysis") :
      ImageAnaBase(name),
      _tree(nullptr)
    {}
    
    virtual ~EmptyAnalysis(){}
    
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

  };

  class EmptyAnalysisFactory : public AlgoFactoryBase {
  public:
    EmptyAnalysisFactory() { AlgoFactory::get().add_factory("EmptyAnalysis",this); }
    ~EmptyAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new EmptyAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

