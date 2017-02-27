#ifndef __SHOWERONEDGE_H__
#define __SHOWERONEDGE_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/LArPlaneGeo.h"
/*
  @brief: estimate the shower vertex point using shower seeds
*/
namespace larocv {
 
  class ShowerOnEdge : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ShowerOnEdge(const std::string name = "ShowerOnEdge") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~ShowerOnEdge(){}
    
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
    LArPlaneGeo _geo;    
  };

  /**
     \class larocv::ShowerOnEdgeFactory
     \brief A concrete factory class for larocv::ShowerOnEdge
   */
  class ShowerOnEdgeFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ShowerOnEdgeFactory() { AlgoFactory::get().add_factory("ShowerOnEdge",this); }
    /// dtor
    ~ShowerOnEdgeFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new ShowerOnEdge(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

