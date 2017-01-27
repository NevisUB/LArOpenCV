#ifndef __VERTEXTRACKCLUSTER_H__
#define __VERTEXTRACKCLUSTER_H__

#include "Geo2D/Core/VectorArray.h"
#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexClusterData.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexParticleCluster.h"

namespace larocv {
 
  class VertexTrackCluster : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexTrackCluster(const std::string name = "VertexTrackCluster") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    virtual ~VertexTrackCluster(){}

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

    VertexParticleCluster _algo;
    AlgorithmID_t _refine2d_algo_id;
    unsigned short _pi_threshold;
    float _contour_pad;
  };
  
  /**
     \class larocv::VertexTrackClusterFactory
     \brief A concrete factory class for larocv::VertexTrackCluster
   */
  class VertexTrackClusterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    VertexTrackClusterFactory() { AlgoFactory::get().add_factory("VertexTrackCluster",this); }
    /// dtor
    ~VertexTrackClusterFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new VertexTrackCluster(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

