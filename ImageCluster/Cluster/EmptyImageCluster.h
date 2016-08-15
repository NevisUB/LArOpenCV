#ifndef __EMPTYIMAGECLUSTER_H__
#define __EMPTYIMAGECLUSTER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class EmptyImageCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class EmptyImageCluster : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    EmptyImageCluster(const std::string name="EmptyImageCluster") : 
       ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~EmptyImageCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);
    
  private:

  };

  class EmptyImageClusterFactory : public ClusterAlgoFactoryBase {
  public:
    EmptyImageClusterFactory() { ClusterAlgoFactory::get().add_factory("EmptyImageCluster",this); }
    ~EmptyImageClusterFactory() {}
    ClusterAlgoBase* create(const std::string instance_name) { return new EmptyImageCluster(instance_name); }
  };

  static EmptyImageClusterFactory __global_EmptyImageClusterFactory__;
}
#endif
/** @} */ // end of doxygen group 

