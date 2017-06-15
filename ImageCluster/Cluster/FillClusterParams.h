//by vic

#ifndef __FILLCLUSTERPARAMS_H__
#define __FILLCLUSTERPARAMS_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
 
  class FillClusterParams : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    FillClusterParams(const std::string name = "FillClusterParams") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    virtual ~FillClusterParams(){}
    
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

  bool _pool ;

  };

  /**
     \class larocv::FillClusterParamsFactory
     \brief A concrete factory class for larocv::FillClusterParams
   */
  class FillClusterParamsFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    FillClusterParamsFactory() { ClusterAlgoFactory::get().add_factory("FillClusterParams",this); }
    /// dtor
    ~FillClusterParamsFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new FillClusterParams(instance_name); }
  };
  /// Global larocv::FillClusterParamsFactory to register ClusterAlgoFactory
  static FillClusterParamsFactory __global_FillClusterParamsFactory__;
  
}
#endif
/** @} */ // end of doxygen group 

