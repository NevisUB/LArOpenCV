//by vic

#ifndef __HIPCLUSTER_H__
#define __HIPCLUSTER_H__

#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoData/HIPClusterData.h"
#include "Geo2D/Core/Vector.h"

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
    void _Configure_(const Config_t &pset);
    
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta,
				       larocv::ROI& roi);
    
  private:

    int _min_hip_cluster_size;
    int _min_mip_cluster_size;

    std::vector<int> _mip_thresh_v;
    std::vector<int> _hip_thresh_v;

    int _dilation_size;
    int _dilation_iter;
    int _blur_size;

    bool _mask_hip;
    
  };

  
  
    /**
       \class larocv::HIPClusterFactory
       \brief A concrete factory class for larocv::HIPCluster
    */
    class HIPClusterFactory : public AlgoFactoryBase {
    public:
      /// ctor
      HIPClusterFactory() { AlgoFactory::get().add_factory("HIPCluster",this); }
      /// dtor
      ~HIPClusterFactory() {}
      /// create method
      ImageClusterBase* create(const std::string instance_name) { return new HIPCluster(instance_name); }
    };
  
}
#endif
/** @} */ // end of doxygen group 

