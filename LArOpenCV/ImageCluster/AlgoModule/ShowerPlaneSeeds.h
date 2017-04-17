#ifndef __SHOWERPLANESEEDS_H__
#define __SHOWERPLANESEEDS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/OneTrackOneShower.h"

/*
  @brief: find a linear track (contour + two 2D edges). Correlate edges across planes and determine 3D edges.
*/
namespace larocv {
 
  class ShowerPlaneSeeds : public larocv::ImageAnaBase {
    
  public:
    
    ShowerPlaneSeeds(const std::string name = "ShowerPlaneSeeds") :
      ImageAnaBase(name)
    {}
    
    virtual ~ShowerPlaneSeeds(){}
    
    void Finalize(TFile*) {}

  protected:

    void Reset()
    {  _OneTrackOneShower.Reset(); }
    
    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const
    { return true; }

  private:
    
    float _threshold;
    OneTrackOneShower _OneTrackOneShower;
    
  };

  class ShowerPlaneSeedsFactory : public AlgoFactoryBase {
  public:
    ShowerPlaneSeedsFactory() { AlgoFactory::get().add_factory("ShowerPlaneSeeds",this); }
    ~ShowerPlaneSeedsFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new ShowerPlaneSeeds(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

