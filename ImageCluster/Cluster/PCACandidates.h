//by vic

#ifndef __PCACANDIDATES_H__
#define __PCACANDIDATES_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"
#include "AlgoData/PCACandidatesData.h"
#include "Core/Geo2D.h"
#include "AlgoClass/PCACrossing.h"

namespace larocv {
 
  class PCACandidates : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    PCACandidates(const std::string name = "PCACandidates") :
      ImageAnaBase(name),
      _PCACrossing()
    {}
    
    /// Default destructor
    virtual ~PCACandidates(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:

    AlgorithmID_t _defect_cluster_algo_id;
    
    bool  _per_vertex;

    PCACrossing _PCACrossing;
  };

  /**
     \class larocv::PCACandidatesFactory
     \brief A concrete factory class for larocv::PCACandidates
  */
  class PCACandidatesFactory : public AlgoFactoryBase {
  public:
    /// ctor
    PCACandidatesFactory() { AlgoFactory::get().add_factory("PCACandidates",this); }
    /// dtor
    ~PCACandidatesFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new PCACandidates(instance_name); }
  };
  
}
#endif
/** @} */ // end of doxygen group 

