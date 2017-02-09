#ifndef __LINEARTRACKSEEDS_H__
#define __LINEARTRACKSEEDS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoData/LinearTrackClusterData.h"
#include "LArOpenCV/ImageCluster/AlgoClass/SingleLinearTrack.h"

namespace larocv {
 
  class LinearTrackSeeds : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    LinearTrackSeeds(const std::string name = "LinearTrackSeeds") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~LinearTrackSeeds(){}

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
     \class larocv::LinearTrackSeedsFactory
     \brief A concrete factory class for larocv::LinearTrackSeeds
   */
  class LinearTrackSeedsFactory : public AlgoFactoryBase {
  public:
    /// ctor
    LinearTrackSeedsFactory() { AlgoFactory::get().add_factory("LinearTrackSeeds",this); }
    /// dtor
    ~LinearTrackSeedsFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new LinearTrackSeeds(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

