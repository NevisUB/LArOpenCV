//by vic

#ifndef __PCACANDIDATES_H__
#define __PCACANDIDATES_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

#include "Core/Geo2D.h"

#include "PCACandidatesData.h"

namespace larocv {
 
  class PCACandidates : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    PCACandidates(const std::string name = "PCACandidates") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    virtual ~PCACandidates(){}

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
    
  };

  /**
     \class larocv::PCACandidatesFactory
     \brief A concrete factory class for larocv::PCACandidates
   */
  class PCACandidatesFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    PCACandidatesFactory() { ClusterAlgoFactory::get().add_factory("PCACandidates",this); }
    /// dtor
    ~PCACandidatesFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new PCACandidates(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new PCACandidatesData(instance_name,id);}
  };
  /// Global larocv::PCACandidatesFactory to register ClusterAlgoFactory
  static PCACandidatesFactory __global_PCACandidatesFactory__;
  
}
#endif
/** @} */ // end of doxygen group 

