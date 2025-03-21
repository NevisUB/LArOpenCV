//by vic

#ifndef __SBCLUSTER_H__
#define __SBCLUSTER_H__

#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {
 
  class PolarCluster : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    PolarCluster(const std::string name = "PolarCluster") :
      ClusterAlgoBase(name),
      _dilation_size ( 5 ),
      _dilation_iter ( 2 ),
      _blur_size_r   ( 5 ),
      _blur_size_t   ( 5 ),
      _thresh        ( 1 ),
      _thresh_maxval (255)
    {}
    
    /// Default destructor
    virtual ~PolarCluster(){}

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

    int _dilation_size;
    int _dilation_iter;

    int _blur_size_r;
    int _blur_size_t;
    
    float _thresh;
    float _thresh_maxval;
    
  };

  /**
     \class larocv::PolarClusterFactory
     \brief A concrete factory class for larocv::PolarCluster
   */
  class PolarClusterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    PolarClusterFactory() { AlgoFactory::get().add_factory("PolarCluster",this); }
    /// dtor
    ~PolarClusterFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new PolarCluster(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

