#ifndef __MATCHANALYSIS_H__
#define __MATCHANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/DefectBreaker.h"
#include "LArOpenCV/ImageCluster/AlgoClass/AtomicAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexAnalysis.h"


/*
  @brief: XXX
*/
namespace larocv {
 
  class MatchAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    MatchAnalysis(const std::string name = "MatchAnalysis") :
      ImageAnaBase(name),
      _tree(nullptr),
      _DefectBreaker(),
      _AtomicAnalysis(),
      _VertexAnalysis()
    {}
    
    virtual ~MatchAnalysis(){}
    
    void Finalize(TFile* fout)
    { fout->cd(); _tree->Write(); }

  protected:

    void Reset() { }

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const { return true; }
    
  private:

    AlgorithmID_t _combined_id;
    AlgorithmID_t _particle_id;

    TTree* _tree;

    int _roid;
    int _vtxid;

    double _x;
    double _y;
    double _z;

    bool _break_contours;
    bool _debug_match_ana;
    
  public:

    std::vector<double> _par_pixel_ratio_v; // c
    std::vector<int>    _par_valid_end_pt_v; // c
    std::vector<double> _par_end_pt_x_v; // c
    std::vector<double> _par_end_pt_y_v; // c
    std::vector<double> _par_end_pt_z_v; // c
    std::vector<double> _par_n_planes_charge_v; // c
    std::vector<double> _par_3d_PCA_theta_estimate_v; // c
    std::vector<double> _par_3d_PCA_phi_estimate_v; // c
    std::vector<double> _par_3d_segment_theta_estimate_v; // c
    std::vector<double> _par_3d_segment_phi_estimate_v; // c
    int _vertex_n_planes_charge; // c

    
    //
    // for debug, per vertex storage
    //
    struct MatchAna {
      std::vector<double> par_pixel_ratio_v; 
      std::vector<int>    par_valid_end_pt_v;
      std::vector<double> par_end_pt_x_v; 
      std::vector<double> par_end_pt_y_v; 
      std::vector<double> par_end_pt_z_v; 
      std::vector<double> par_n_planes_charge_v; 
      std::vector<double> par_3d_PCA_theta_estimate_v;
      std::vector<double> par_3d_PCA_phi_estimate_v; 
      std::vector<double> par_3d_segment_theta_estimate_v;
      std::vector<double> par_3d_segment_phi_estimate_v;
      int vertex_n_planes_charge; 
    };
    
    std::vector<MatchAna> _match_ana_v;
    
  private:
    void Clear();

  public:
    DefectBreaker _DefectBreaker;
    AtomicAnalysis _AtomicAnalysis;
    VertexAnalysis _VertexAnalysis;

  private:
    // theta and phi
    std::pair<float,float> Angle3D(const data::Vertex3D& vtx1, const data::Vertex3D& vtx2);
    std::pair<float,float> Angle3D(const data::Particle& particle,
				   const std::vector<cv::Mat>& img_v,
				   const data::Vertex3D& start3d = data::Vertex3D());


    // for debug
    void StoreMatchAna();
    void ClearMatchAna() { _match_ana_v.clear(); }
    
  };

  class MatchAnalysisFactory : public AlgoFactoryBase {
  public:
    MatchAnalysisFactory() { AlgoFactory::get().add_factory("MatchAnalysis",this); }
    ~MatchAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new MatchAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 
