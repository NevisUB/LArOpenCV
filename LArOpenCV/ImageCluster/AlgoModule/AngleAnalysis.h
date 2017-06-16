#ifndef __ANGLEANALYSIS_H__
#define __ANGLEANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "Geo2D/Core/Circle.h"

/*
  @brief: Calculate dangle of two coming particles
*/
namespace larocv {
 
  class AngleAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    AngleAnalysis(const std::string name = "AngleAnalysis") :
      ImageAnaBase(name),
      _tree(nullptr)
      {}
    
    virtual ~AngleAnalysis(){}
    
    void Finalize(TFile* fout) {
      fout->cd();
      _tree->Write();
    }

    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:

    void Clear();
    int _nplanes;
    int _pixels_number;
    int _unity;
    double _angle_cut;
    AlgorithmID_t _combined_vertex_analysis_algo_id;
    AlgorithmID_t _particle_id;
    
    TTree* _tree;

    int _roid;
    int _vtxid;
    int _nparticles;
    
    double _x;
    double _y;
    double _z;

    double _anglediff;
    double _anglediff_straight;
    double _anglediff_max;
    
    std::vector<double> _anglediff_v;//Per plane
    std::vector<double> _angle_particles;
    int _straightness;

    
  public:
    void ParticleAngle(GEO2D_Contour_t ctor_origin, 
		       GEO2D_Contour_t ctor, 
		       geo2d::Circle<float> circle, 
		       double& pct, double& angle);
    
    double Getx2vtxmean(GEO2D_Contour_t ctor, float x2d, float y2d, double& pct);
    double Gety2vtxmean(GEO2D_Contour_t ctor, float x2d, float y2d, double& pct);
    
  };

  class AngleAnalysisFactory : public AlgoFactoryBase {
  public:
    AngleAnalysisFactory() { AlgoFactory::get().add_factory("AngleAnalysis",this); }
    ~AngleAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new AngleAnalysis
	(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

