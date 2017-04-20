#ifndef __EMPTYALGOMODULE_H__
#define __EMPTYALGOMODULE_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: An EmptyAlgoModule
*/
namespace larocv {
 
  class EmptyAlgoModule : public larocv::ImageAnaBase {
    
  public:
    
    EmptyAlgoModule(const std::string name = "EmptyAlgoModule") :
      ImageAnaBase(name)
    {}
    
    virtual ~EmptyAlgoModule(){}
    
    void Finalize(TFile*) {}

    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:

  };

  class EmptyAlgoModuleFactory : public AlgoFactoryBase {
  public:
    EmptyAlgoModuleFactory() { AlgoFactory::get().add_factory("EmptyAlgoModule",this); }
    ~EmptyAlgoModuleFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new EmptyAlgoModule(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

