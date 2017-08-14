#ifndef __DQDSANALYSIS_H__
#define __DQDSANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/AtomicAnalysis.h"
#include "TVector3.h"

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

    float Correct3D ( float dqds_mean, float theta, float phi);

    std::vector<float> Sort01 (const std::vector<float> input_dqds);
    
  private:

    void Clear();

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
    double _particle0_end_x;
    double _particle0_end_y;
    double _particle0_end_z;
    double _particle1_end_x;
    double _particle1_end_y;
    double _particle1_end_z;
    
    std::vector<float> _dqds_0_v;//dqds mean per vtx, per plane, particle 0
    std::vector<float> _dqds_1_v;//dqds mean per vtx, per plane, particle 1
    std::vector<float> _dqds_diff_v;//diff of mean dqds between particle 0&1
    std::vector<float> _dqds_ratio_v;//diff of mean dqds between particle 0&1
    float _dqds_diff_01;//smaller dqds diff of plane 0&1
    float _dqds_ratio_01;//smaller dqds diff of plane 0&1
    
    std::vector<float> _t_dqds_0_v;//dqds mean per vtx, per plane, particle 0
    std::vector<float> _t_dqds_1_v;//dqds mean per vtx, per plane, particle 1
    std::vector<float> _t_dqds_diff_v;//diff of mean dqds between particle 0&1
    std::vector<float> _t_dqds_ratio_v;//diff of mean dqds between particle 0&1
    float _t_dqds_diff_01;//smaller dqds diff of plane 0&1
    float _t_dqds_ratio_01;//smaller dqds diff of plane 0&1

    std::vector<float> _r_dqds_0_v;//dqds spectrum per vtx, per plane, particle 0
    std::vector<float> _r_dqds_1_v;//dqds spectrum per vtx, per plane, particle 1
    std::vector<float> _r_dqds_diff_v;//diff of mean dqds between particle 0&1
    std::vector<float> _r_dqds_ratio_v;//diff of mean dqds between particle 0&1
    float _r_dqds_diff_01;//smaller dqds diff of plane 0&1
    float _r_dqds_ratio_01;//smaller dqds diff of plane 0&1
    
    std::vector<float> _dqdx_0_v_3dc;//per vtx, per plane, particle 0
    std::vector<float> _dqdx_1_v_3dc;//per vtx, per plane, particle 1
    std::vector<float> _dqdx_diff_v_3dc;//diff of mean dqds between particle 0&1
    std::vector<float> _dqdx_ratio_v_3dc;//diff of mean dqds between particle 0&1

    std::vector<float> _dqdx_0_end_v_3dc;//per vtx, per plane, particle 0
    std::vector<float> _dqdx_1_end_v_3dc;//per vtx, per plane, particle 1
    std::vector<float> _dqdx_diff_end_v_3dc;//diff of mean dqds between particle 0&1
    std::vector<float> _dqdx_ratio_end_v_3dc;//diff of mean dqds between particle 0&1
    
    std::vector<float> _trackp_dqds_v;//per vtx, per plane, track particle
    std::vector<float> _showerp_dqds_v;//per vtx, per plane, shower particle
    std::vector<float> _trackp_dqdx_3dc_v;//per vtx, per plane, track particle
    std::vector<float> _showerp_dqdx_3dc_v;//per vtx, per plane, shower particle

    std::vector<float> _long_trackp_dqds_v;//per vtx, per plane, long track particle
    std::vector<float> _short_trackp_dqds_v;//per vtx, per plane, short particle
    std::vector<float> _long_trackp_dqdx_3dc_v;//per vtx, per plane, long track particle
    std::vector<float> _short_trackp_dqdx_3dc_v;//per vtx, per plane, short particle
    
    std::vector<std::pair<float, float>> _vertex_v;
    std::vector<std::pair<float, float>> _particle0_end_point;
    std::vector<std::pair<float, float>> _particle1_end_point;
    std::vector<std::vector<std::pair<float, float>>> _particle0_pixels_v;
    std::vector<std::vector<std::pair<float, float>>> _particle1_pixels_v;
   
    
    std::vector<std::vector<TVector3>> _image_array_tmp;
    
    std::vector<float> _image_particle0_plane0_tmp_x;
    std::vector<float> _image_particle0_plane0_tmp_y;
    std::vector<float> _image_particle0_plane0_tmp_v;
    std::vector<float> _image_particle0_plane1_tmp_x;
    std::vector<float> _image_particle0_plane1_tmp_y;
    std::vector<float> _image_particle0_plane1_tmp_v;
    std::vector<float> _image_particle0_plane2_tmp_x;
    std::vector<float> _image_particle0_plane2_tmp_y;
    std::vector<float> _image_particle0_plane2_tmp_v;
    
    std::vector<float> _image_particle1_plane0_tmp_x;
    std::vector<float> _image_particle1_plane0_tmp_y;
    std::vector<float> _image_particle1_plane0_tmp_v;
    std::vector<float> _image_particle1_plane1_tmp_x;
    std::vector<float> _image_particle1_plane1_tmp_y;
    std::vector<float> _image_particle1_plane1_tmp_v;
    std::vector<float> _image_particle1_plane2_tmp_x;
    std::vector<float> _image_particle1_plane2_tmp_y;
    std::vector<float> _image_particle1_plane2_tmp_v;
    
    
    //Variables for truncated mean
    double _window_size;
    int _window_size_thre;
    double _head_frac;
    double _tail_frac;
    
    size_t _drop_location;//drop the first x data points on dqds spectrum
    
    float _dqds_scan_thre;
    
    float _theta_0;
    float _phi_0;
    float _theta_1;
    float _phi_1;

    float _dqdx_diff_01_3dc;//smaller dqds diff of plane 0&1 [3D corrected]
    float _dqdx_ratio_01_3dc;//smaller dqds diff of plane 0&1 [3D corrected]

    float _trackp_totq;
    float _showerp_totq;
    float _trackp_cosz;
    float _showerp_cosz;

    float _dedx_p0;
    float _dedx_p1;

    float _dedx_diff;

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

