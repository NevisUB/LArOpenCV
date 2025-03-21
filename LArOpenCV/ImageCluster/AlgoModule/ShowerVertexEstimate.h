#ifndef __SHOWERVERTEXESTIMATE_H__
#define __SHOWERVERTEXESTIMATE_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoData/VertexSeed.h"
#include "LArOpenCV/ImageCluster/AlgoClass/OneTrackOneShower.h"
/*
  @brief: estimate the shower vertex point using shower seeds
*/
namespace larocv {
 
  class ShowerVertexEstimate : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    ShowerVertexEstimate(const std::string name = "ShowerVertexEstimate") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~ShowerVertexEstimate(){}
    
    void Reset();

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}
    
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();

    bool _PostProcess_() const;

  private:
    OneTrackOneShower _OneTrackOneShower;
    AlgorithmID_t _algo_id_vertex_seed;
    AlgorithmID_t _algo_id_vertex_scan_seed;
    AlgorithmID_t _algo_id_shower_track_vertex;
    AlgorithmID_t _algo_id_track_vertex_estimate;
    AlgorithmID_t _algo_id_track_vertex_particle_cluster;
  };

  /**
     \class larocv::ShowerVertexEstimateFactory
     \brief A concrete factory class for larocv::ShowerVertexEstimate
   */
  class ShowerVertexEstimateFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ShowerVertexEstimateFactory() { AlgoFactory::get().add_factory("ShowerVertexEstimate",this); }
    /// dtor
    ~ShowerVertexEstimateFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new ShowerVertexEstimate(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

