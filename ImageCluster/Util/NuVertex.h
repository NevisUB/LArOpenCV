#ifndef __NUVERTEX_H__
#define __NUVERTEX_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
 
  class NuVertex : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    NuVertex(const std::string name = "NuVertex") :
      ClusterAlgoBase(name),
      _block_size    ( 5 ),
      _aperture_size ( 2 ),
      _harris_k      ( 5 )
    {}
    
    /// Default destructor
    virtual ~NuVertex(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi);

    int _ctr;
    
  private:

    int _block_size;
    int _aperture_size;
    double _harris_k;
    double _threshold;

    int _dilation_size;
    int _dilation_iter;

    int _blur_size_r;
    int _blur_size_t;
    
    float _thresh;
    float _thresh_maxval;

  };

  /**
     \class larocv::NuVertexFactory
     \brief A concrete factory class for larocv::NuVertex
   */
  class NuVertexFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    NuVertexFactory() { ClusterAlgoFactory::get().add_factory("NuVertex",this); }
    /// dtor
    ~NuVertexFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new NuVertex(instance_name); }
  };
  /// Global larocv::NuVertexFactory to register ClusterAlgoFactory
  static NuVertexFactory __global_NuVertexFactory__;
  
}
#endif
/** @} */ // end of doxygen group 

