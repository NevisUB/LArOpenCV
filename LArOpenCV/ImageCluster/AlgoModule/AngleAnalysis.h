#ifndef __ANGLEANALYSIS_H__
#define __ANGLEANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: Calculate Δangle of two coming particles
*/
namespace larocv {
 
  class AngleAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    AngleAnalysis(const std::string name = "AngleAnalysis") :
      ImageAnaBase(name)
    {}
    
    virtual ~AngleAnalysis(){}
    
    void Finalize(TFile*) {}

    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:

    int _nplanes;
    
    AlgorithmID_t _combined_vertex_analysis_algo_id;
  };

  class AngleAnalysisFactory : public AlgoFactoryBase {
  public:
    AngleAnalysisFactory() { AlgoFactory::get().add_factory("AngleAnalysis",this); }
    ~AngleAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new AngleAnalysis
	(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

