#ifndef __MATCHOVERLAP_H__
#define __MATCHOVERLAP_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataUtils.h"
#include "LArOpenCV/ImageCluster/AlgoClass/VertexAnalysis.h"

/*
  @brief: An MatchOverlap
*/
namespace larocv {

  
  class MatchOverlap : public larocv::ImageAnaBase {
    
  public:
    
    MatchOverlap(const std::string name = "MatchOverlap") :
      ImageAnaBase(name),
      _VertexAnalysis()
    {}
    
    virtual ~MatchOverlap(){}
    
    void Finalize(TFile*) {}

    void Reset() {}

  protected:

    void _Configure_(const Config_t &pset);
    
    void _Process_();
    
    bool _PostProcess_() const;

  private:
    
    AlgorithmID_t _vertex_algo_id;
    AlgorithmID_t _particle_cluster_algo_id;
    size_t _vertex_algo_offset;
    size_t _particle_cluster_algo_offset;

    float _match_coverage;
    float _match_particles_per_plane;
    float _match_min_number;
    bool _match_check_type;
    bool _match_weight_by_size;

    VertexAnalysis _VertexAnalysis;
    
  };

  class MatchOverlapFactory : public AlgoFactoryBase {
  public:
    MatchOverlapFactory() { AlgoFactory::get().add_factory("MatchOverlap",this); }
    ~MatchOverlapFactory() {}
    ImageClusterBase* create(const std::string instance_name) { return new MatchOverlap(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

