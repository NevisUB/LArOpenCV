//by vic

#ifndef __EDGEVERTEXCANDIDATES_H__
#define __EDGEVERTEXCANDIDATES_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"
#include "Core/Geo2D.h"
#include "AlgoData/EdgeVertexCandidatesData.h"

namespace larocv {
 
  class EdgeVertexCandidates : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    EdgeVertexCandidates(const std::string name = "EdgeVertexCandidates") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~EdgeVertexCandidates(){}

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

  };

  /**
     \class larocv::EdgeVertexCandidatesFactory
     \brief A concrete factory class for larocv::EdgeVertexCandidates
  */
  class EdgeVertexCandidatesFactory : public AlgoFactoryBase {
  public:
    /// ctor
    EdgeVertexCandidatesFactory() { AlgoFactory::get().add_factory("EdgeVertexCandidates",this); }
    /// dtor
    ~EdgeVertexCandidatesFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new EdgeVertexCandidates(instance_name); }
    /// data create method
    data::AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new data::EdgeVertexCandidatesData(instance_name,id);}
  };
  
}
#endif
/** @} */ // end of doxygen group 

