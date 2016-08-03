//by ariana  

#ifndef __SBCLUSTERMAN_H__
#define __SBCLUSTERMAN_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
 
  class SBManCluster : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    SBManCluster(const std::string name = "SBManCluster") :
      ClusterAlgoBase(name),
      _dilation_size ( 5 ),
      _dilation_iter ( 2 ),
      _blur_size     ( 5 ),
      _thresh        ( 1 ),
      _thresh_maxval (255)
    {}
    
    /// Default destructor
    virtual ~SBManCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

    //void onMouse( int event, int x, int y, int f ) ; //, void* ) ;

    static void mouseCallback(int event, int x, int y, int flags, void* userData);

    void showImage() ;

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta);
  private:

    int _dilation_size;
    int _dilation_iter;
    int _blur_size;
    float _thresh;
    float _thresh_maxval;

    
  };

void onMouse( int event, int x, int y, int f, void* ) ;


  /**
     \class larocv::SBManClusterFactory
     \brief A concrete factory class for larocv::SBManCluster
   */
  class SBManClusterFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    SBManClusterFactory() { ClusterAlgoFactory::get().add_factory("SBManCluster",this); }
    /// dtor
    ~SBManClusterFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new SBManCluster(instance_name); }
  };
  /// Global larocv::SBManClusterFactory to register ClusterAlgoFactory
  static SBManClusterFactory __global_SBManClusterFactory__;
  
}
#endif
/** @} */ // end of doxygen group 

