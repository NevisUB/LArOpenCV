#ifndef __LINEARTRACKCLUSTER_H__
#define __LINEARTRACKCLUSTER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrackClusterData.h"
#include "LArOpenCV/ImageCluster/AlgoClass/SingleLinearTrack.h"

namespace larocv {
 
  class LinearTrackCluster : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    LinearTrackCluster(const std::string name = "LinearTrackCluster") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~LinearTrackCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    void Reset()
    { _algo.Reset(); }

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:
    SingleLinearTrack _algo;
    AlgorithmID_t _algo_id_part;
  };

  /**
     \class larocv::LinearTrackClusterFactory
     \brief A concrete factory class for larocv::LinearTrackCluster
   */
  class LinearTrackClusterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    LinearTrackClusterFactory() { AlgoFactory::get().add_factory("LinearTrackCluster",this); }
    /// dtor
    ~LinearTrackClusterFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new LinearTrackCluster(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

