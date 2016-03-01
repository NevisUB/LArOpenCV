//by vic

#ifndef __SBCLUSTER_H__
#define __SBCLUSTER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
 
  class SBCluster : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larcv::ImageClusterManager
    SBCluster(const std::string name = "SBCluster") :
      ClusterAlgoBase(name),
      _dilation_size ( 5 ),
      _dilation_iter ( 2 ),
      _blur_size     ( 5 ),
      _thresh        ( 1 ),
      _thresh_maxval (255)
    {}
    
    /// Default destructor
    virtual ~SBCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    larcv::Cluster2DArray_t _Process_(const larcv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larcv::ImageMeta& meta);
    
  private:

    int _dilation_size;
    int _dilation_iter;
    int _blur_size;
    float _thresh;
    float _thresh_maxval;
    
  };

  /**
     \class larcv::SBClusterFactory
     \brief A concrete factory class for larcv::SBCluster
   */
  class SBClusterFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    SBClusterFactory() { ClusterAlgoFactory::get().add_factory("SBCluster",this); }
    /// dtor
    ~SBClusterFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new SBCluster(instance_name); }
  };
  /// Global larcv::SBClusterFactory to register ClusterAlgoFactory
  static SBClusterFactory __global_SBClusterFactory__;
  
}
#endif
/** @} */ // end of doxygen group 

