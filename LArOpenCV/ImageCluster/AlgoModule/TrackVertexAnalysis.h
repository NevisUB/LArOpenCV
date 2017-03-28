#ifndef __TRACKVERTEXANALYSIS_H__
#define __TRACKVERTEXANALYSIS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexAnalysis.h"


/*
  @brief: analyze track vertex
*/
namespace larocv {
 
  class TrackVertexAnalysis : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    TrackVertexAnalysis(const std::string name = "TrackVertexAnalysis") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~TrackVertexAnalysis(){}
    
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
    VertexAnalysis _vertexana;
    

    AlgorithmID_t _track_vertex_algo_id;
    AlgorithmID_t _track_particle_algo_id;
    float _min_time_wire_3d;
    uint _required_xs_planes;
    uint _required_xs;
    bool _compute_dqdx;
    
  };

  /**
     \class larocv::TrackVertexAnalysisFactory
     \brief A concrete factory class for larocv::TrackVertexAnalysis
   */
  class TrackVertexAnalysisFactory : public AlgoFactoryBase {
  public:
    /// ctor
    TrackVertexAnalysisFactory() { AlgoFactory::get().add_factory("TrackVertexAnalysis",this); }
    /// dtor
    ~TrackVertexAnalysisFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new TrackVertexAnalysis(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

