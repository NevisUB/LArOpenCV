#ifndef __SECONDSHOWERANALYSIS_H__
#define __SECONDSHOWERANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PixelScan3D.h"

/*
  @brief: XXX
*/
namespace larocv {
 
  class SecondShowerAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    SecondShowerAnalysis(const std::string name = "SecondShowerAnalysis") :
      ImageAnaBase(name),
      _tree(nullptr)
    {}
    
    virtual ~SecondShowerAnalysis(){}
    
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

    void ClearVertex();
    void ResetVectors(size_t sz);

    float _shower_frac;
    float _shower_size;
    float _shower_impact;

    int _secondshower;
    int _shr_rad_pts;

    float _shr_theta;
    float _shr_phi;

    float _shr_half_dist;
    float _shr_min_dist;
    float _shr_imp_dist;

  public:
    PixelScan3D _PixelScan3D;
    
    std::vector<std::vector<data::Vertex3D> > _reg_vv;
    std::vector<GEO2D_ContourArray_t > _actor_vv;

    std::pair<float,float> _angle;
    std::array<float,3> _avector;
    std::array<float,3> _mean_pos;
    float _dist;
    
    inline bool CompareAsses(const std::array<size_t,3> & a1, const std::array<size_t,3> & a2)
    {  return (true ? (( a1[0] == a2[0]) && (a1[1] == a2[1]) && (a1[2] == a2[2])) : false); }
    
  };

  class SecondShowerAnalysisFactory : public AlgoFactoryBase {
  public:
    SecondShowerAnalysisFactory() { AlgoFactory::get().add_factory("SecondShowerAnalysis",this); }
    ~SecondShowerAnalysisFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new SecondShowerAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

