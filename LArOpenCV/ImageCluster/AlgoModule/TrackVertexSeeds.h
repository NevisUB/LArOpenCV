#ifndef __TRACKVERTEXSEEDS_H__
#define __TRACKVERTEXSEEDS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

#include "LArOpenCV/ImageCluster/AlgoClass/ClusterHIPMIP.h"
#include "LArOpenCV/ImageCluster/AlgoClass/DefectBreaker.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PCACrossing.h"
/*
  @brief: Created 2D vertex seeds from defect points and PCA crossing points
*/
namespace larocv {

  class TrackVertexSeeds : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    TrackVertexSeeds(const std::string name = "TrackVertexSeeds") :
      ImageAnaBase(name),
      _ClusterHIPMIP(),
      _DefectBreaker(),
      _PCACrossing()
    {}
    
    /// Default destructor
    virtual ~TrackVertexSeeds(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    void _Process_();

    bool _PostProcess_() const { return true; }

  public:
    const ClusterHIPMIP& ClusterHIPMIPAlgo() { return _ClusterHIPMIP; }
    const DefectBreaker& DefectBreakerAlgo() { return _DefectBreaker; }
    const PCACrossing& PCACrossingAlgo()     { return _PCACrossing;   }
    
  private:
    
    ClusterHIPMIP _ClusterHIPMIP;
    DefectBreaker _DefectBreaker;
    PCACrossing   _PCACrossing;
    
  };

  
  /**
     \class TrackVertexSeedsFactory
     \brief A concrete factory class for TrackVertexSeeds
  */
  class TrackVertexSeedsFactory : public AlgoFactoryBase {
  public:
    /// ctor
    TrackVertexSeedsFactory() { AlgoFactory::get().add_factory("TrackVertexSeeds",this); }
    /// dtor
    ~TrackVertexSeedsFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new TrackVertexSeeds(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

