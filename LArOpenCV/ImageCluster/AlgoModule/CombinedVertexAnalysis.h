#ifndef __COMBINEDVERTEXANALYSIS_H__
#define __COMBINEDVERTEXANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: analyze shower vertex
*/
namespace larocv {
 
  class CombinedVertexAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    CombinedVertexAnalysis(const std::string name = "CombinedVertexAnalysis") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~CombinedVertexAnalysis(){}
    
    void Reset();

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}
    
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi){}

    bool _PostProcess_(std::vector<cv::Mat>& img_v);

  private:

    AlgorithmID_t _shower_vertex_algo_id;
    AlgorithmID_t _track_vertex_algo_id;
    AlgorithmID_t _track_vertex_filter_algo_id;
    size_t _nplanes;

    bool _remove_superseded;
  };
  
  /**
     \class larocv::CombinedVertexAnalysisFactory
     \brief A concrete factory class for larocv::CombinedVertexAnalysis
   */
  class CombinedVertexAnalysisFactory : public AlgoFactoryBase {
  public:
    /// ctor
    CombinedVertexAnalysisFactory() { AlgoFactory::get().add_factory("CombinedVertexAnalysis",this); }
    /// dtor
    ~CombinedVertexAnalysisFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new CombinedVertexAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

