#ifndef __MATCHOVERLAP_H__
#define __MATCHOVERLAP_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

//
// Match algos
//
#include "LArOpenCV/ImageCluster/AlgoClass/MatchAlgoOverlap.h"
#include "LArOpenCV/ImageCluster/AlgoClass/MatchAlgoTimeIOU.h"


/*
  @brief: An MatchOverlap
*/
namespace larocv {

  
  class MatchOverlap : public larocv::ImageAnaBase {
    
  public:
    
    MatchOverlap(const std::string name = "MatchOverlap") :
      ImageAnaBase(name)
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

    MatchAlgoBase *_alg;

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

