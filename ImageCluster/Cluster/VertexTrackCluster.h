//by vic

#ifndef __VERTEXTRACKCLUSTER_H__
#define __VERTEXTRACKCLUSTER_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"
#include "Core/VectorArray.h"

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

    float _mask_radius;
    AlgorithmID_t _vtx_algo_id;
    
  };
  
  class VertexTrackClusterData : public larocv::AlgoDataBase {
  public:
    VertexTrackClusterData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~VertexTrackClusterData(){}

    void Clear() {
      _ctor_vv.clear();
      _ctor_vv.resize(3);
    }

    std::vector< std::vector< geo2d::VectorArray<int> > > _ctor_vv;

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
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new VertexTrackClusterData(instance_name,id);}
  };
  
}
#endif
/** @} */ // end of doxygen group 

