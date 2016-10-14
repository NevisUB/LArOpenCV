//by vic

#ifndef __HIPCLUSTER_H__
#define __HIPCLUSTER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

#include "Core/Vector.h"

#include "HIPClusterData.h"

namespace larocv {
 
  class HIPCluster : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    HIPCluster(const std::string name = "HIPCluster") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    virtual ~HIPCluster(){}

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

    int _min_hip_cluster_size;
    int _min_mip_cluster_size;
    int _mip_thresh;
    int _hip_thresh;
    
  };

  /**
     \class larocv::HIPClusterFactory
     \brief A concrete factory class for larocv::HIPCluster
   */
  class HIPClusterFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    HIPClusterFactory() { ClusterAlgoFactory::get().add_factory("HIPCluster",this); }
    /// dtor
    ~HIPClusterFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new HIPCluster(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new HIPClusterData(instance_name,id);}
  };
  /// Global larocv::HIPClusterFactory to register ClusterAlgoFactory
  static HIPClusterFactory __global_HIPClusterFactory__;
  
}
#endif
/** @} */ // end of doxygen group 

