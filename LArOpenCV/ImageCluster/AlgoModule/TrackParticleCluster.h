#ifndef __TRACKPARTICLECLUSTER_H__
#define __TRACKPARTICLECLUSTER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexParticleCluster.h"
#include "LArOpenCV/ImageCluster/AlgoClass/DefectBreaker.h"
#include "LArOpenCV/ImageCluster/AlgoClass/AtomicAnalysis.h"

namespace larocv {
 
  class TrackParticleCluster : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    TrackParticleCluster(const std::string name = "TrackParticleCluster") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~TrackParticleCluster(){}
    
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
    DefectBreaker _DefectBreaker;
    AtomicAnalysis _AtomicAnalysis;
    
    AlgorithmID_t _track_vertex_estimate_algo_id;
    AlgorithmID_t _track_super_cluster_algo_id;
    unsigned short _pi_threshold;
    float _contour_pad;
  };

  /**
     \class larocv::TrackParticleClusterFactory
     \brief A concrete factory class for larocv::TrackParticleCluster
   */
  class TrackParticleClusterFactory : public AlgoFactoryBase {

  public:
    /// ctor
    TrackParticleClusterFactory() { AlgoFactory::get().add_factory("TrackParticleCluster",this); }
    /// dtor
    ~TrackParticleClusterFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new TrackParticleCluster(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

