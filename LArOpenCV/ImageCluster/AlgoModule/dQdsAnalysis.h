#ifndef __DQDSANALYSIS_H__
#define __DQDSANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/AtomicAnalysis.h"

/*
  @brief: An dQdsAnalysis
*/
namespace larocv {
 
  class dQdsAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    dQdsAnalysis(const std::string name = "dQdsAnalysis") :
      ImageAnaBase(name),
      _tree(nullptr)
      {}
    
    virtual ~dQdsAnalysis(){}
    
    void Finalize(TFile* fout) {      
      fout->cd(); _tree->Write();}

    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:

    void Clear();

    int _nplanes;
    
    AlgorithmID_t _angle_analysis_algo_id;
    AlgorithmID_t _combined_vertex_analysis_algo_id;

    AtomicAnalysis _AtomicAnalysis;

    TTree* _tree;//Per RSEERV[run, subrun, event, entry, roi, vertex]
    
    int _roid;
    int _vtxid;
    int _nparticles;
    
    double _x;
    double _y;
    double _z;
    
    std::vector<std::vector<std::vector<float>>> _dqds_vvv;//per vtx, per plane, per particle
    std::vector<float> _dqds_diff_v;
  };

  class dQdsAnalysisFactory : public AlgoFactoryBase {
  public:
    dQdsAnalysisFactory() { AlgoFactory::get().add_factory("dQdsAnalysis",this); }
    ~dQdsAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new dQdsAnalysis
	(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

