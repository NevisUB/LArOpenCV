#ifndef __TRACKANALYSIS_H__
#define __TRACKANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {

  class TrackAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    TrackAnalysis(const std::string name = "TrackAnalysis") :
      ImageAnaBase(name),
      _SuperClusterer()
    {}
    
    /// Default destructor
    virtual ~TrackAnalysis(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    void _Process_(const Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   ImageMeta& meta,
		   ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v) { return true; }
    
  private:

  };

  
  /**
     \class TrackAnalysisFactory
     \brief A concrete factory class for TrackAnalysis
  */
  class TrackAnalysisFactory : public AlgoFactoryBase {
  public:
    /// ctor
    TrackAnalysisFactory() { AlgoFactory::get().add_factory("TrackAnalysis",this); }
    /// dtor
    ~TrackAnalysisFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new TrackAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

