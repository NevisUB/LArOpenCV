#ifndef __SHOWERANALYSIS_H__
#define __SHOWERANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
/*
  @brief: analyze shower vertex
*/
namespace larocv {
 
  class ShowerAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ShowerAnalysis(const std::string name = "ShowerAnalysis") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~ShowerAnalysis(){}
    
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
     \class larocv::ShowerAnalysisFactory
     \brief A concrete factory class for larocv::ShowerAnalysis
   */
  class ShowerAnalysisFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ShowerAnalysisFactory() { AlgoFactory::get().add_factory("ShowerAnalysis",this); }
    /// dtor
    ~ShowerAnalysisFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new ShowerAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

