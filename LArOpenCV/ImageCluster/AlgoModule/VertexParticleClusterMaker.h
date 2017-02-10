#ifndef __VERTEXPARTICLECLUSTERMAKER_H__
#define __VERTEXPARTICLECLUSTERMAKER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoClass/DefectBreaker.h"
#include "LArOpenCV/ImageCluster/AlgoClass/AtomicAnalysis.h"

namespace larocv {
 
  class VertexParticleClusterMaker : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexParticleClusterMaker(const std::string name = "VertexParticleClusterMaker") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~VertexParticleClusterMaker(){}
    
    void Reset() {}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:
    VertexParticleCluster _VertexParticleCluster;
    
    AlgorithmID_t _vertex_estimate_algo_id;
    AlgorithmID_t _super_cluster_algo_id;
    unsigned short _pi_threshold;
    float _contour_pad;

    bool _create_compound;
    DefectBreaker _DefectBreaker;
    AtomicAnalysis _AtomicAnalysis;
  };

  /**
     \class larocv::VertexParticleClusterMakerFactory
     \brief A concrete factory class for larocv::VertexParticleClusterMaker
   */
  class VertexParticleClusterMakerFactory : public AlgoFactoryBase {

  public:
    /// ctor
    VertexParticleClusterMakerFactory() { AlgoFactory::get().add_factory("VertexParticleClusterMaker",this); }
    /// dtor
    ~VertexParticleClusterMakerFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new VertexParticleClusterMaker(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

