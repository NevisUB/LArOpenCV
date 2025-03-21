#ifndef __DQDSANALYSIS_H__
#define __DQDSANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/AtomicAnalysis.h"

/*
  @brief: R. An dQdsAnalysis
*/
namespace larocv {
 
  class dQdsAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    dQdsAnalysis(const std::string name = "dQdsAnalysis") :
      ImageAnaBase(name),
      _tree(nullptr)
      {}
    
    virtual ~dQdsAnalysis() {}
    
    void Finalize(TFile* fout) {
      fout->cd(); 
      _tree->Write();
    }
    
    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;
    
    std::vector<float> PeakFinder  (std::vector<float> input_dqds, double frac); //Find the first peak in dqds and stop

    float Correct3D ( float dqds_mean, float theta, float phi);

    std::vector<float> Sort01 (const std::vector<float> input_dqds);

    void AssignRatio (const std::vector<float> input, float& output);
    
    void AssignDiff (const std::vector<float> input, float& output);
  
      
  private:

    void ClearEvent();
    void ClearVertex();

    int _nplanes;
    
    AlgorithmID_t _angle_analysis_algo_id;
    AlgorithmID_t _combined_vertex_analysis_algo_id;
    AlgorithmID_t _match_analysis_algo_id;
    AlgorithmID_t _match_overlap_algo_id;
    AlgorithmID_t _shape_analysis_algo_id;
    
    AtomicAnalysis _AtomicAnalysis;

    TTree* _tree;//Per RSEERV[run, subrun, event, entry, roi, vertex]
    
    int _roid;
    int _vtxid;
    
    double _x;
    double _y;
    double _z;
    
    // raw dqds in the vertex circle
    std::vector<float> _dqds_0_v;    // trunk dqds mean per vtx, per plane, particle 0
    std::vector<float> _dqds_1_v;    // trunk dqds mean per vtx, per plane, particle 1
    std::vector<float> _dqds_diff_v; // diff of mean trunk dqds between particle 0&1
    std::vector<float> _dqds_ratio_v;// ratio of mean trunk dqds between particle 0&1
    float _dqds_diff_01;             // larger mean trunk dqds diff of plane 0&1
    float _dqds_ratio_01;            // smaller mean trunk dqds diff of plane 0&1
    
    std::vector<float> _r_dqds_0_v;
    std::vector<float> _r_dqds_1_v;
    std::vector<float> _r_dqds_diff_v;
    std::vector<float> _r_dqds_ratio_v;
    float _r_dqds_diff_01;
    float _r_dqds_ratio_01;
    
    std::vector<float> _s_dqds_0_v;
    std::vector<float> _s_dqds_1_v;
    std::vector<float> _s_dqds_diff_v;
    std::vector<float> _s_dqds_ratio_v;
    float _s_dqds_diff_01;
    float _s_dqds_ratio_01;

    std::vector<float> _t_dqds_0_v;   
    std::vector<float> _t_dqds_1_v;
    std::vector<float> _t_dqds_diff_v;
    std::vector<float> _t_dqds_ratio_v;
    float _t_dqds_diff_01;
    float _t_dqds_ratio_01;
    
    std::vector<float> _dqdx_0_v_3dc;
    std::vector<float> _dqdx_1_v_3dc;
    std::vector<float> _dqdx_diff_v_3dc;
    std::vector<float> _dqdx_ratio_v_3dc;
    float _dqdx_diff_01_3dc;
    float _dqdx_ratio_01_3dc;

    std::vector<float> _dqdx_0_end_v_3dc;
    std::vector<float> _dqdx_1_end_v_3dc;
    std::vector<float> _dqdx_diff_end_v_3dc;
    std::vector<float> _dqdx_ratio_end_v_3dc;
    
    std::vector<float> _trackp_dqds_v;
    std::vector<float> _showerp_dqds_v;
    std::vector<float> _trackp_dqdx_3dc_v;
    std::vector<float> _showerp_dqdx_3dc_v;

    std::vector<float> _long_trackp_dqds_v;
    std::vector<float> _short_trackp_dqds_v;
    std::vector<float> _long_trackp_dqdx_3dc_v;
    std::vector<float> _short_trackp_dqdx_3dc_v;
    
    std::vector<float> _par1_0_qden_scan_v;
    std::vector<float> _par1_1_qden_scan_v;
    std::vector<float> _par1_2_qden_scan_v;
    std::vector<float> _par2_0_qden_scan_v;
    std::vector<float> _par2_1_qden_scan_v;
    std::vector<float> _par2_2_qden_scan_v;

    std::vector<float> _npx_in_vtx_circ_v;
    std::vector<float> _npx_on_vtx_circ_v;
    
    //Variables for truncated mean
    double _window_size;
    int _window_size_thre;
    double _window_frac;
    double _head_frac;
    double _tail_frac;
    
    float _dqds_scan_thre;
    
    float _theta_0;
    float _phi_0;
    float _theta_1;
    float _phi_1;

    float _trackp_totq;
    float _showerp_totq;
    float _trackp_cosz;
    float _showerp_cosz;

    float _dedx_p0;
    float _dedx_p1;

    float _dedx_diff;
    float _trackp_dedx;
    float _showerp_dedx;
    float _long_trackp_dedx;
    float _short_trackp_dedx;

    float _chi2_par1_0;
    float _chi2_par1_1;
    float _chi2_par1_2;
    float _chi2_par2_0;
    float _chi2_par2_1;
    float _chi2_par2_2;
    
  };

  class dQdsAnalysisFactory : public AlgoFactoryBase {
  public:
    dQdsAnalysisFactory() { AlgoFactory::get().add_factory("dQdsAnalysis",this); }
    ~dQdsAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new dQdsAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

