#ifndef __SIMPLECOLINEAR_H__
#define __SIMPLECOLINEAR_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoData/SimpleColinearData.h"

namespace larocv {
 
  class SimpleColinear : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    SimpleColinear(const std::string name = "SimpleColinear") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~SimpleColinear(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:
    
  };

  /**
     \class larocv::SimpleColinearFactory
     \brief A concrete factory class for larocv::SimpleColinear
  */
  class SimpleColinearFactory : public AlgoFactoryBase {
  public:
    /// ctor
    SimpleColinearFactory() { AlgoFactory::get().add_factory("SimpleColinear",this); }
    /// dtor
    ~SimpleColinearFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new SimpleColinear(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

