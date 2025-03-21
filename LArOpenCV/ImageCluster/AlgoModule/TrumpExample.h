#ifndef __TRUMPEXAMPLE_H__
#define __TRUMPEXAMPLE_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: An TrumpExample
*/
namespace larocv {
 
  class TrumpExample : public larocv::ImageAnaBase {
    
  public:
    
    TrumpExample(const std::string name = "TrumpExample") :
      ImageAnaBase(name)
    {}
    
    virtual ~TrumpExample(){}
    
    void Finalize(TFile*) {}

    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:

  };

  class TrumpExampleFactory : public AlgoFactoryBase {
  public:
    TrumpExampleFactory() { AlgoFactory::get().add_factory("TrumpExample",this); }
    ~TrumpExampleFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new TrumpExample
	(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

