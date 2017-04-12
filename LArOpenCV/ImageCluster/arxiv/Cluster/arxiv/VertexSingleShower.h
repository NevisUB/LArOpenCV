#ifndef __VERTEXSINGLESHOWER_H__
#define __VERTEXSINGLESHOWER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoClass/OneTrackOneShower.h"
#include "LArOpenCV/ImageCluster/AlgoClass/ElectronShowerVertexSeed.h"

namespace larocv {
 
  class VertexSingleShower : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    VertexSingleShower(const std::string name = "VertexSingleShower") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~VertexSingleShower(){}

    /// Finalize after process
    void Finalize(TFile*) {}

    const std::vector<larocv::data::Vertex3D>& ScannedVertex3D() const
    { return _algo.ScannedVertex3D(); }
    
    const std::vector<std::vector<larocv::data::CircleVertex> >& ScannedVertex2D() const
    { return _algo.ScannedVertex2D(); }

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);

    void Reset();
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);
    
  private:

    OneTrackOneShower _algo;
    ElectronShowerVertexSeed _algo_seed;
    AlgorithmID_t _algo_id_dqdx;
    AlgorithmID_t _algo_id_vertex_track;
    AlgorithmID_t _algo_id_linear_track;
    
  };

  /**
     \class larocv::VertexSingleShowerFactory
     \brief A concrete factory class for larocv::VertexSingleShower
   */
  class VertexSingleShowerFactory : public AlgoFactoryBase {
  public:
    /// ctor
    VertexSingleShowerFactory() { AlgoFactory::get().add_factory("VertexSingleShower",this); }
    /// dtor
    ~VertexSingleShowerFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new VertexSingleShower(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

