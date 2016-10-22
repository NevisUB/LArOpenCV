//by vic

#ifndef __VERTEXTRACKCLUSTER_H__
#define __VERTEXTRACKCLUSTER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
 
  class VertexTrackCluster : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexTrackCluster(const std::string name = "VertexTrackCluster") :
      ClusterAlgoBase(name),
      _dilation_size ( 5 ),
      _dilation_iter ( 2 ),
      _blur_size_r   ( 5 ),
      _blur_size_t   ( 5 ),
      _thresh        ( 1 ),
      _thresh_maxval (255)
    {}
    
    /// Default destructor
    virtual ~VertexTrackCluster(){}

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

    int _dilation_size;
    int _dilation_iter;
    
    int _blur_size_r;
    int _blur_size_t;
    
    float _thresh;
    float _thresh_maxval;
    
  };

  /**
     \class larocv::VertexTrackClusterFactory
     \brief A concrete factory class for larocv::VertexTrackCluster
   */
  class VertexTrackClusterFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    VertexTrackClusterFactory() { ClusterAlgoFactory::get().add_factory("VertexTrackCluster",this); }
    /// dtor
    ~VertexTrackClusterFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new VertexTrackCluster(instance_name); }
  };
  /// Global larocv::VertexTrackClusterFactory to register ClusterAlgoFactory
  static VertexTrackClusterFactory __global_VERTEXTRACKClusterFactory__;
  
}
#endif
/** @} */ // end of doxygen group 

