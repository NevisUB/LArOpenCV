#ifndef __TRACKANALYSIS_H__
#define __TRACKANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
/*
  @brief: analyze vertex
*/
namespace larocv {
 
  class TrackAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    TrackAnalysis(const std::string name = "TrackAnalysis") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~TrackAnalysis(){}
    
    void Reset();

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}
    
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi){}

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:
  };

  /**
     \class larocv::TrackAnalysisFactory
     \brief A concrete factory class for larocv::TrackAnalysis
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

