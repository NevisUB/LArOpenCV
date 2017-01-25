//by vic

#ifndef __VERTEXSEEDS_H__
#define __VERTEXSEEDS_H__

#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

#include "LArOpenCV/ImageCluster/AlgoClass/ClusterHIPMIP.h"
#include "LArOpenCV/ImageCluster/AlgoClass/DefectBreaker.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PCACrossing.h"

#include "LArOpenCV/ImageCluster/AlgoData/VertexSeedsData.h"

namespace larocv {

  class VertexSeeds : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexSeeds(const std::string name = "VertexSeeds") :
      ClusterAlgoBase(name),
      _ClusterHIPMIP(),
      _DefectBreaker(),
      _PCACrossing()
    {}
    
    /// Default destructor
    virtual ~VertexSeeds(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    Cluster2DArray_t _Process_(const Cluster2DArray_t& clusters,
			       const ::cv::Mat& img,
			       ImageMeta& meta,
			       ROI& roi);
    
  private:
    
    ClusterHIPMIP _ClusterHIPMIP;
    DefectBreaker _DefectBreaker;
    PCACrossing   _PCACrossing;
    
  };

  
  /**
     \class VertexSeedsFactory
     \brief A concrete factory class for VertexSeeds
  */
  class VertexSeedsFactory : public AlgoFactoryBase {
  public:
    /// ctor
    VertexSeedsFactory() { AlgoFactory::get().add_factory("VertexSeeds",this); }
    /// dtor
    ~VertexSeedsFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new VertexSeeds(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

