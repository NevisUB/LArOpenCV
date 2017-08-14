#ifndef __GAPANALYSIS_H__
#define __GAPANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: An GapAnalysis
*/
namespace larocv {
 
  class GapAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    GapAnalysis(const std::string name = "GapAnalysis") :
      ImageAnaBase(name), _tree(nullptr)
    {}
    
    virtual ~GapAnalysis(){}
    
    void Finalize(TFile *fout) 
    { fout->cd(); _tree->Write(); }
    
    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:

    void ClearEvent();
    void ClearVertex();
    
    AlgorithmID_t _combined_id;
    AlgorithmID_t _particle_id;
    
    TTree* _tree;
    
    int _roid;
    int _vtxid;

    double _x;   
    double _y; 
    double _z;
    
    int _pathexists1;
    int _pathexists2;
    
    std::vector<int> _pathexists1_v;
    std::vector<int> _pathexists2_v;
    
    int _infiducial;
  };

  class GapAnalysisFactory : public AlgoFactoryBase {
  public:
    GapAnalysisFactory() { AlgoFactory::get().add_factory("GapAnalysis",this); }
    ~GapAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new GapAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

