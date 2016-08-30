//by vic

#ifndef __SBCLUSTER_H__
#define __SBCLUSTER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
 
  class PolarCluster : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    PolarCluster(const std::string name = "PolarCluster") :
      ClusterAlgoBase(name),
      _dilation_size ( 5 ),
      _dilation_iter ( 2 ),
      _blur_size     ( 5 ),
      _thresh        ( 1 ),
      _thresh_maxval (255)
    {}
    
    /// Default destructor
    virtual ~PolarCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi);
    
  private:

    int _dilation_size;
    int _dilation_iter;
    int _blur_size;
    float _thresh;
    float _thresh_maxval;
    
  };

  /**
     \class larocv::PolarClusterFactory
     \brief A concrete factory class for larocv::PolarCluster
   */
  class PolarClusterFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    PolarClusterFactory() { ClusterAlgoFactory::get().add_factory("PolarCluster",this); }
    /// dtor
    ~PolarClusterFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new PolarCluster(instance_name); }
  };
  /// Global larocv::PolarClusterFactory to register ClusterAlgoFactory
  static PolarClusterFactory __global_PolarClusterFactory__;
  
}
#endif
/** @} */ // end of doxygen group 

