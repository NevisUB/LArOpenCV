#ifndef __MATCHANALYSIS_H__
#define __MATCHANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/DefectBreaker.h"
#include "LArOpenCV/ImageCluster/AlgoClass/AtomicAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexAnalysis.h"
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"

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

    float _trunk_radius;
    
  public:

    std::vector<double> _par_pixel_ratio_v; 
    std::vector<int>    _par_valid_end_pt_v; 
    std::vector<double> _par_end_pt_x_v; 
    std::vector<double> _par_end_pt_y_v; 
    std::vector<double> _par_end_pt_z_v; 
    std::vector<double> _par_n_planes_charge_v; 

    std::vector<double> _par_3d_segment_theta_estimate_v; 
    std::vector<double> _par_3d_segment_phi_estimate_v; 
    int _vertex_n_planes_charge;
    int _vertex_n_planes_near_dead;
    int _vertex_n_planes_on_dead; 
    
    std::vector<double> _par_pca_theta_estimate_v; 
    std::vector<double> _par_pca_phi_estimate_v; 
    std::vector<float>  _par_pca_end_x_v;  
    std::vector<float>  _par_pca_end_y_v;  
    std::vector<float>  _par_pca_end_z_v;
    std::vector<int>    _par_pca_end_in_fiducial_v;
    std::vector<float>  _par_pca_end_len_v;
    std::vector<int>    _par_pca_valid_v;
    
    std::vector<double> _par_trunk_pca_theta_estimate_v; 
    std::vector<double> _par_trunk_pca_phi_estimate_v; 
    std::vector<float>  _par_trunk_pca_end_x_v;  
    std::vector<float>  _par_trunk_pca_end_y_v;  
    std::vector<float>  _par_trunk_pca_end_z_v;
    std::vector<int>    _par_trunk_pca_end_in_fiducial_v;
    std::vector<float>  _par_trunk_pca_end_len_v;
    std::vector<int>    _par_trunk_pca_valid_v;
    
    float _trunk_length;
    float _vertex_charge_radius;
    
  private:

    void Clear();
    void ResizeVectors(size_t npar);
    
    std::pair<float,float> Angle3D(const data::Vertex3D& vtx1,
				   const data::Vertex3D& vtx2);

    std::vector<data::SpacePoint> SpacePointsEstimate(const data::Particle& particle,
						      const std::vector<cv::Mat>& img_v,
						      const std::vector<cv::Mat>& qimg_v,
						      const float radius=0.0,
						      const data::Vertex3D vertex = data::Vertex3D());

    std::pair<float,float> Angle3D(const std::vector<data::SpacePoint>& vtx3d_v,
				   const data::Vertex3D& start3d);

    std::pair<float,float> Angle3D(const data::Particle& particle,
				   const std::vector<cv::Mat>& img_v,
				   const std::vector<cv::Mat>& qimg_v,
				   const data::Vertex3D& start3d,
				   const float radius=0.0);

    std::array<float,3> EndPoint3D(const std::vector<data::SpacePoint>& space_pts_v,
				   const float theta, const float phi,
				   const data::Vertex3D& start_pt);
    
    float Distance3D(const std::array<float,3>& pt1,const data::Vertex3D& vtx);
    
    float Distance3D(const data::Vertex3D& vtx, const std::array<float,3>& pt1);
    
    
  public:
    DefectBreaker _DefectBreaker;
    AtomicAnalysis _AtomicAnalysis;
    VertexAnalysis _VertexAnalysis;


  };
  class MatchAnalysisFactory : public AlgoFactoryBase {
  public:
    MatchAnalysisFactory() { AlgoFactory::get().add_factory("MatchAnalysis",this); }
    ~MatchAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new MatchAnalysis(instance_name); }
  };

    
}

#endif
