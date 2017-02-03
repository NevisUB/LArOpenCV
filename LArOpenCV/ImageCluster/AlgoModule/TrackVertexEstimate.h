#ifndef __TRACKVERTEXESTIMATE_H__
#define __TRACKVERTEXESTIMATE_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexEstimateData.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeedsData.h"
#include "LArOpenCV/ImageCluster/AlgoClass/TrackVertexScan2D.h"

//#include "LArOpenCV/ImageCluster/AlgoData/Refine2DVertexData.h"


namespace larocv {
 
  class TrackVertexEstimate : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    TrackVertexEstimate(const std::string name = "TrackVertexEstimate") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~TrackVertexEstimate(){}
    
    void Reset()
    { _algo.Reset(); }

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}
    
    const TrackVertexScan2D& Algo() const { return _algo; }
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:

    AlgorithmID_t _vertex_seed_algo_id;
    
    TrackVertexScan2D _algo;
  };

  /**
     \class larocv::TrackVertexEstimateFactory
     \brief A concrete factory class for larocv::TrackVertexEstimate
   */
  class TrackVertexEstimateFactory : public AlgoFactoryBase {
  public:
    /// ctor
    TrackVertexEstimateFactory() { AlgoFactory::get().add_factory("TrackVertexEstimate",this); }
    /// dtor
    ~TrackVertexEstimateFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new TrackVertexEstimate(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

