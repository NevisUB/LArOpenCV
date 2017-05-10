#ifndef __SHAPEANALYSIS_H__
#define __SHAPEANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

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
    
    bool _PostProcess_() const;
    
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
    std::vector<float> _track_frac_v;
    std::vector<float> _shower_frac_v;
    std::vector<float> _mean_pixel_dist_v;
    std::vector<float> _sigma_pixel_dist_v;
    std::vector<float> _angular_sum_v;

    void Clear();
    
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

