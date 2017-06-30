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
    
    std::vector<float> PeakFinder  (std::vector<float> input_dqds, double frac); //Find the first peak in dqds and stop

    std::vector<float> dQdsDropper (std::vector<float> input_dqds);
    
  private:

    void Clear();

    int _nplanes;
    
    AlgorithmID_t _angle_analysis_algo_id;
    AlgorithmID_t _combined_vertex_analysis_algo_id;
    AlgorithmID_t _match_analysis_algo_id;
    
    AtomicAnalysis _AtomicAnalysis;

    TTree* _tree;//Per RSEERV[run, subrun, event, entry, roi, vertex]
    
    int _roid;
    int _vtxid;
    
    double _x;
    double _y;
    double _z;
    
    std::vector<float> _dqds_0_v;//per vtx, per plane, particle 0
    std::vector<float> _dqds_1_v;//per vtx, per plane, particle 1
    std::vector<float> _dqds_diff_v;//diff of mean dqds between particle 0&1
    std::vector<float> _dqds_ratio_v;//diff of mean dqds between particle 0&1
    
    std::vector<float> _t_dqds_0_v;//per vtx, per plane, particle 0
    std::vector<float> _t_dqds_1_v;//per vtx, per plane, particle 1
    std::vector<float> _t_dqds_diff_v;//diff of mean dqds between particle 0&1
    std::vector<float> _t_dqds_ratio_v;//diff of mean dqds between particle 0&1

    //Variables for truncated mean
    double _window_size;
    int _window_size_thre;
    double _window_frac;
    
    size_t _chop_size;//chop the first x data points on dqds spectrum
    
    float _dqds_scan_thre;
    
    float _dqds_diff_01;//smaller dqds diff of plane 0&1
    float _dqds_ratio_01;//smaller dqds diff of plane 0&1

    float _theta;
    float _phi;
    
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

