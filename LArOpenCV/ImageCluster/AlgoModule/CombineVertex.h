#ifndef __COMBINEVERTEX_H__
#define __COMBINEVERTEX_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

/*
  @brief: analyze shower vertex
*/
namespace larocv {
 
  class CombineVertex : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    CombineVertex(const std::string name = "CombineVertex") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~CombineVertex(){}
    
    void Reset();

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}
    
    
  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();

    bool _PostProcess_() const;

  private:

    AlgorithmID_t _shower_vertex_algo_id;
    AlgorithmID_t _track_vertex_algo_id;
    AlgorithmID_t _particle_cluster_algo_id;
    size_t _nplanes;
  };
  
  /**
     \class larocv::CombineVertexFactory
     \brief A concrete factory class for larocv::CombineVertex
   */
  class CombineVertexFactory : public AlgoFactoryBase {
  public:
    /// ctor
    CombineVertexFactory() { AlgoFactory::get().add_factory("CombineVertex",this); }
    /// dtor
    ~CombineVertexFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new CombineVertex(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

