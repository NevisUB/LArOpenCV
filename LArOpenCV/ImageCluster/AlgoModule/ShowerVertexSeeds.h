#ifndef __SHOWERVERTEXSEEDS_H__
#define __SHOWERVERTEXSEEDS_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/ElectronShowerVertexSeed.h"
/*
  @brief: determine 3D shower vertex seeds from 3D edge points, 3D track vertex points, and 3D end points
*/
namespace larocv {

  class ShowerVertexSeeds : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ShowerVertexSeeds(const std::string name = "ShowerVertexSeeds") :
      ImageAnaBase(name),
      _ElectronShowerVertexSeed()
    {
      _vertex3d_id = kINVALID_ALGO_ID;
      _super_cluster_id = kINVALID_ALGO_ID;
      _linear_track_id = kINVALID_ALGO_ID;
      _compound_id = kINVALID_ALGO_ID;
    }
    
    /// Default destructor
    virtual ~ShowerVertexSeeds(){}

    /// Finalize after process
    void Finalize(TFile*) {}

    void Reset() { _ElectronShowerVertexSeed.Reset(); }

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    void _Process_(const Cluster2DArray_t& clusters,
		   ::cv::Mat& img,
		   ImageMeta& meta,
		   ROI& roi)
    { _ElectronShowerVertexSeed.SetPlaneInfo(meta); }

    bool _PostProcess_(std::vector<cv::Mat>& img_v);
    
  private:

    AlgorithmID_t _vertex3d_id;
    AlgorithmID_t _super_cluster_id;
    AlgorithmID_t _linear_track_id;
    AlgorithmID_t _compound_id;
    ElectronShowerVertexSeed _ElectronShowerVertexSeed;
  };

  /**
     \class ShowerVertexSeedsFactory
     \brief A concrete factory class for ShowerVertexSeeds
  */
  class ShowerVertexSeedsFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ShowerVertexSeedsFactory() { AlgoFactory::get().add_factory("ShowerVertexSeeds",this); }
    /// dtor
    ~ShowerVertexSeedsFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new ShowerVertexSeeds(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 
