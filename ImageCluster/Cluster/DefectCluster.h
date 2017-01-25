//by vic

#ifndef __DEFECTCLUSTER_H__
#define __DEFECTCLUSTER_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"

#include "Core/Geo2D.h"
#include "AlgoData/DefectClusterData.h"
#include "AlgoClass/DefectBreaker.h"

namespace larocv {

  class DefectCluster : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    DefectCluster(const std::string name = "DefectCluster") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    virtual ~DefectCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    Cluster2DArray_t _Process_(const Cluster2DArray_t& clusters,
			       const ::cv::Mat& img,
			       ImageMeta& meta,
			       ROI& roi);
    

  private:
    AlgorithmID_t _vertextrack_algo_id;
    AlgorithmID_t _lineartrack_algo_id;

    DefectBreaker _DefectBreaker;
    
  };

  
  /**
     \class DefectClusterFactory
     \brief A concrete factory class for DefectCluster
  */
  class DefectClusterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    DefectClusterFactory() { AlgoFactory::get().add_factory("DefectCluster",this); }
    /// dtor
    ~DefectClusterFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new DefectCluster(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

