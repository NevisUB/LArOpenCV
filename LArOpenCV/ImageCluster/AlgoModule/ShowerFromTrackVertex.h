#ifndef __SHOWERFROMTRACKVERTEX_H__
#define __SHOWERFROMTRACKVERTEX_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
/*
  @brief: estimate the shower vertex point using shower seeds
*/
namespace larocv {
 
  class ShowerFromTrackVertex : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ShowerFromTrackVertex(const std::string name = "ShowerFromTrackVertex") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~ShowerFromTrackVertex(){}
    
    void Reset();

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}
    
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:
    size_t _vertex3d_id;
    size_t _compound_id;
    LArPlaneGeo _geo;    
    double _overlap_fraction;
    double _circle_default_radius;
  };

  /**
     \class larocv::ShowerFromTrackVertexFactory
     \brief A concrete factory class for larocv::ShowerFromTrackVertex
   */
  class ShowerFromTrackVertexFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ShowerFromTrackVertexFactory() { AlgoFactory::get().add_factory("ShowerFromTrackVertex",this); }
    /// dtor
    ~ShowerFromTrackVertexFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new ShowerFromTrackVertex(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

