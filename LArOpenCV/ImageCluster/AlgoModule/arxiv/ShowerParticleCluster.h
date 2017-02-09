#ifndef __SHOWERPARTICLECLUSTER_H__
#define __SHOWERPARTICLECLUSTER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoData/SingleShowerData.h"
#include "LArOpenCV/ImageCluster/AlgoClass/OneTrackOneShower.h"

namespace larocv {
 
  class ShowerParticleCluster : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ShowerParticleCluster(const std::string name = "ShowerParticleCluster") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~ShowerParticleCluster(){}
    
    void Reset() {}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

    const std::vector<larocv::data::Vertex3D>& ScannedVertex3D() const
    { return _algo.ScannedVertex3D(); }
    
    const std::vector<std::vector<larocv::data::CircleVertex> >& ScannedVertex2D() const
    { return _algo.ScannedVertex2D(); }

    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:
    AlgorithmID_t _algo_id_shower_estimate;
    AlgorithmID_t _algo_id_shower_super_cluster;
    OneTrackOneShower _algo;
  };

  /**
     \class larocv::ShowerParticleClusterFactory
     \brief A concrete factory class for larocv::ShowerParticleCluster
   */
  class ShowerParticleClusterFactory : public AlgoFactoryBase {

  public:
    /// ctor
    ShowerParticleClusterFactory() { AlgoFactory::get().add_factory("ShowerParticleCluster",this); }
    /// dtor
    ~ShowerParticleClusterFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new ShowerParticleCluster(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

