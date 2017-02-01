#ifndef __VERTEXSEEDS_H__
#define __VERTEXSEEDS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

#include "LArOpenCV/ImageCluster/AlgoClass/ClusterHIPMIP.h"
#include "LArOpenCV/ImageCluster/AlgoClass/DefectBreaker.h"
#include "LArOpenCV/ImageCluster/AlgoClass/PCACrossing.h"

#include "LArOpenCV/ImageCluster/AlgoData/VertexSeedsData.h"

namespace larocv {

  class VertexSeeds : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexSeeds(const std::string name = "VertexSeeds") :
      ImageAnaBase(name),
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
    
    void _Process_(const Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   ImageMeta& meta,
		   ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v) { return true; }
    
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

