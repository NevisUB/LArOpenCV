#ifndef __SHAPEANALYSIS_H__
#define __SHAPEANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: XXX
*/
namespace larocv {
 
  class ShapeAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    ShapeAnalysis(const std::string name = "ShapeAnalysis") :
      ImageAnaBase(name)
    {}
    
    virtual ~ShapeAnalysis(){}
    
    void Finalize(TFile*) {}

  protected:

    void Reset() { }

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const
    { return true; }

  private:

    AlgorithmID_t _combined_id;
    
  };

  class ShapeAnalysisFactory : public AlgoFactoryBase {
  public:
    ShapeAnalysisFactory() { AlgoFactory::get().add_factory("ShapeAnalysis",this); }
    ~ShapeAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new ShapeAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

