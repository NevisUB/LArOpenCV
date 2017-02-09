#ifndef __TRACKSUPERCLUSTER_H__
#define __TRACKSUPERCLUSTER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/SuperClusterer.h"

namespace larocv {

  class TrackSuperCluster : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    TrackSuperCluster(const std::string name = "TrackSuperCluster") :
      ImageAnaBase(name),
      _SuperClusterer()
    {}
    
    /// Default destructor
    virtual ~TrackSuperCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    void _Process_(const Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   ImageMeta& meta,
		   ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v) { return true; }
    
  private:

    SuperClusterer _SuperClusterer;
  };

  
  /**
     \class TrackSuperClusterFactory
     \brief A concrete factory class for TrackSuperCluster
  */
  class TrackSuperClusterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    TrackSuperClusterFactory() { AlgoFactory::get().add_factory("TrackSuperCluster",this); }
    /// dtor
    ~TrackSuperClusterFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new TrackSuperCluster(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

