#ifndef __SHOWERVERTEXANALYSIS_H__
#define __SHOWERVERTEXANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/ClusterMerge.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"

/*
  @brief: analyze shower vertex
*/
namespace larocv {
 
  class ShowerVertexAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ShowerVertexAnalysis(const std::string name = "ShowerVertexAnalysis") :
      ImageAnaBase(name),
      _ClusterMerge()
    {}
    
    /// Default destructor
    ~ShowerVertexAnalysis(){}
    
    void Reset();

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}
    
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();

    bool _PostProcess_() const;

  private:
    AlgorithmID_t _scluster_algo_id;
    AlgorithmID_t _shower_vertex_algo_id;
    AlgorithmID_t _track_vertex_algo_id;
    AlgorithmID_t _acluster_algo_id;
    
    ClusterMerge _ClusterMerge;
    bool _merge;
    bool _input_adc_img;
    
  };

  /**
     \class larocv::ShowerVertexAnalysisFactory
     \brief A concrete factory class for larocv::ShowerVertexAnalysis
   */
  class ShowerVertexAnalysisFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ShowerVertexAnalysisFactory() { AlgoFactory::get().add_factory("ShowerVertexAnalysis",this); }
    /// dtor
    ~ShowerVertexAnalysisFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new ShowerVertexAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

