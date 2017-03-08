#ifndef __SHOWERVERTEXMERGER_H__
#define __SHOWERVERTEXMERGER_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: analyze shower vertex
*/
namespace larocv {
 
  class ShowerVertexMerger : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ShowerVertexMerger(const std::string name = "ShowerVertexMerger") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~ShowerVertexMerger(){}
    
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
    AlgorithmID_t _track_vertex_algo_id;
    AlgorithmID_t _shower_vertex_algo_id;
    AlgorithmID_t _track_vertex_particle_algo_id;
    AlgorithmID_t _shower_vertex_track_particle_algo_id;
    AlgorithmID_t _shower_vertex_shower_particle_algo_id;
    AlgorithmID_t _shower_super_cluster_maker_algo_id;
    bool _merge_compatible_showers;
  };

  /**
     \class larocv::ShowerVertexMergerFactory
     \brief A concrete factory class for larocv::ShowerVertexMerger
   */
  class ShowerVertexMergerFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ShowerVertexMergerFactory() { AlgoFactory::get().add_factory("ShowerVertexMerger",this); }
    /// dtor
    ~ShowerVertexMergerFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new ShowerVertexMerger(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

