#ifndef __CHEAPVERTEXFINDER_H__
#define __CHEAPVERTEXFINDER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: An YourTrumpExample
*/
namespace larocv {
 
  class CheapVertexFinder : public larocv::ImageAnaBase {
    
  public:
    
  CheapVertexFinder(const std::string name = "CheapVertexFinder") :
    ImageAnaBase(name)
    {}
    
    virtual ~CheapVertexFinder(){}
    
    void Finalize(TFile*) {}

    void Reset() {}


    std::vector<cv::Point> _closest_pt_v;
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:

  };

  class CheapVertexFinderFactory : public AlgoFactoryBase {
  public:
    CheapVertexFinderFactory() { AlgoFactory::get().add_factory("CheapVertexFinder",this); }
    ~CheapVertexFinderFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new CheapVertexFinder
	(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

