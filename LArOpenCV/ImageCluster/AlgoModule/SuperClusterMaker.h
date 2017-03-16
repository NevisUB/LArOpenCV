#ifndef __SUPERCLUSTERMAKER_H__
#define __SUPERCLUSTERMAKER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/SuperClusterer.h"
/*
  @brief: create overall contour and store into algodata
*/
namespace larocv {

  class SuperClusterMaker : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    SuperClusterMaker(const std::string name = "SuperClusterMaker") :
      ImageAnaBase(name),
      _SuperClusterer()
    {}
    
    /// Default destructor
    virtual ~SuperClusterMaker(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    void _Process_(const Cluster2DArray_t& clusters,
		   ::cv::Mat& img,
		   ImageMeta& meta,
		   ROI& roi);

    bool _PostProcess_(std::vector<cv::Mat>& img_v) { return true; }
    
  private:

    SuperClusterer _SuperClusterer;
  };

  
  /**
     \class SuperClusterMakerFactory
     \brief A concrete factory class for SuperClusterMaker
  */
  class SuperClusterMakerFactory : public AlgoFactoryBase {
  public:
    /// ctor
    SuperClusterMakerFactory() { AlgoFactory::get().add_factory("SuperClusterMaker",this); }
    /// dtor
    ~SuperClusterMakerFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new SuperClusterMaker(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

