#ifndef __SHAPEANALYSIS_H__
#define __SHAPEANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoData/InfoCollection.h"

/*
  @brief: XXX
*/
namespace larocv {
 
  class ShapeAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    ShapeAnalysis(const std::string name = "ShapeAnalysis") :
      ImageAnaBase(name),
      _tree(nullptr)
    {}
    
    virtual ~ShapeAnalysis(){}
    
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
    int _nparticles;

    double _x;
    double _y;
    double _z;
    
    std::vector<int>   _nplanes_v;
    std::vector<float> _length_v;
    std::vector<float> _width_v;
    std::vector<float> _perimeter_v;
    std::vector<float> _area_v;
    std::vector<float> _npixel_v;
    std::vector<float> _qsum_v;
    std::vector<float> _track_frac_v;
    std::vector<float> _shower_frac_v;
    std::vector<float> _mean_pixel_dist_v;
    std::vector<float> _sigma_pixel_dist_v;
    std::vector<float> _angular_sum_v;
    std::vector<float> _triangle_d_sum_v;

    std::vector<float> _length_max_v;
    std::vector<float> _width_max_v;
    std::vector<float> _perimeter_max_v;
    std::vector<float> _area_max_v;
    std::vector<float> _npixel_max_v;
    std::vector<float> _qsum_max_v;
    std::vector<float> _track_frac_max_v;
    std::vector<float> _shower_frac_max_v;
    std::vector<float> _mean_pixel_dist_max_v;
    std::vector<float> _sigma_pixel_dist_max_v;
    std::vector<float> _angular_sum_max_v;
    std::vector<float> _triangle_d_max_v;

    std::vector<float> _length_min_v;
    std::vector<float> _width_min_v;
    std::vector<float> _perimeter_min_v;
    std::vector<float> _area_min_v;
    std::vector<float> _npixel_min_v;
    std::vector<float> _qsum_min_v;
    std::vector<float> _track_frac_min_v;
    std::vector<float> _shower_frac_min_v;
    std::vector<float> _mean_pixel_dist_min_v;
    std::vector<float> _sigma_pixel_dist_min_v;
    std::vector<float> _angular_sum_min_v;
    std::vector<float> _triangle_d_min_v;

    std::vector<float> _length_U_v;
    std::vector<float> _width_U_v;
    std::vector<float> _perimeter_U_v;
    std::vector<float> _area_U_v;
    std::vector<float> _npixel_U_v;
    std::vector<float> _qsum_U_v;
    std::vector<float> _track_frac_U_v;
    std::vector<float> _shower_frac_U_v;
    std::vector<float> _mean_pixel_dist_U_v;
    std::vector<float> _sigma_pixel_dist_U_v;
    std::vector<float> _angular_sum_U_v;
    std::vector<float> _triangle_d_U_v;

    std::vector<float> _length_V_v;
    std::vector<float> _width_V_v;
    std::vector<float> _perimeter_V_v;
    std::vector<float> _area_V_v;
    std::vector<float> _npixel_V_v;
    std::vector<float> _qsum_V_v;
    std::vector<float> _track_frac_V_v;
    std::vector<float> _shower_frac_V_v;
    std::vector<float> _mean_pixel_dist_V_v;
    std::vector<float> _sigma_pixel_dist_V_v;
    std::vector<float> _angular_sum_V_v;
    std::vector<float> _triangle_d_V_v;

    std::vector<float> _length_Y_v;
    std::vector<float> _width_Y_v;
    std::vector<float> _perimeter_Y_v;
    std::vector<float> _area_Y_v;
    std::vector<float> _npixel_Y_v;
    std::vector<float> _qsum_Y_v;
    std::vector<float> _track_frac_Y_v;
    std::vector<float> _shower_frac_Y_v;
    std::vector<float> _mean_pixel_dist_Y_v;
    std::vector<float> _sigma_pixel_dist_Y_v;
    std::vector<float> _angular_sum_Y_v;
    std::vector<float> _triangle_d_Y_v;
    
    int _track_par_max_id;
    int _shower_par_max_id;

    float _track_par_max_frac;
    float _shower_par_max_frac;

    int _par1_type;
    int _par2_type;

    float _par1_frac;
    float _par2_frac;

    int _vertex_type;
    
    void ClearEvent();
    void ClearVertex();
    void FillInfo2D(size_t parid, data::Info2D& info);

  };

  class ShapeAnalysisFactory : public AlgoFactoryBase {
  public:
    ShapeAnalysisFactory() { AlgoFactory::get().add_factory("ShapeAnalysis",this); }
    ~ShapeAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new ShapeAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

