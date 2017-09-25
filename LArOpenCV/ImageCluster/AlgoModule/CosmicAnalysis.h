#ifndef __COSMICANALYSIS_H__
#define __COSMICANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: XXX
*/
namespace larocv {
 
  class CosmicAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    CosmicAnalysis(const std::string name = "CosmicAnalysis") :
      ImageAnaBase(name),
      _tree(nullptr)
    {}
    
    virtual ~CosmicAnalysis(){}
    
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

    double _x;
    double _y;
    double _z;

    AlgorithmID_t _particle_id;
    AlgorithmID_t _combined_id;
    AlgorithmID_t _shape_analysis_algo_id;

    void ClearVertex();
    void ResetVectors(size_t sz);

  private:
    std::vector<float> _charge_asym_v;
    std::vector<float> _npixel_asym_v;
    std::vector<float> _ctor_asym_v;
    std::vector<float> _area_asym_v;
    std::vector<float> _frac_stopmu_v;
    std::vector<float> _frac_thrumu_v;

    std::vector<float> _charge_asym_max_v;
    std::vector<float> _npixel_asym_max_v;
    std::vector<float> _ctor_asym_max_v;
    std::vector<float> _area_asym_max_v;
    /* std::vector<float> _frac_stopmu_max_v; */
    /* std::vector<float> _frac_thrumu_max_v; */

    std::vector<float> _charge_asym_min_v;
    std::vector<float> _npixel_asym_min_v;
    std::vector<float> _ctor_asym_min_v;
    std::vector<float> _area_asym_min_v;
    /* std::vector<float> _frac_stopmu_min_v; */
    /* std::vector<float> _frac_thrumu_min_v; */

    float _hull_area_overlap_avg;
    float _hull_area_overlap_min;
    float _hull_area_overlap_max;
    int _hull_nplanes;
    
    std::vector<double> _shape_match_I1_v;
    std::vector<double> _shape_match_I2_v;
    std::vector<double> _shape_match_I3_v;

    float _dtheta_sum_p0;
    float _dtheta_sum_p1;
    float _dtheta_sum_p2;

    float _dtheta_mean_p0;
    float _dtheta_mean_p1;
    float _dtheta_mean_p2;
    
    float _dtheta_xs_p0;
    float _dtheta_xs_p1;
    float _dtheta_xs_p2;

  };

  class CosmicAnalysisFactory : public AlgoFactoryBase {
  public:
    CosmicAnalysisFactory() { AlgoFactory::get().add_factory("CosmicAnalysis",this); }
    ~CosmicAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new CosmicAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

