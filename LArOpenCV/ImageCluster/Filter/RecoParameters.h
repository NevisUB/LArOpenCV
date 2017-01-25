/**
 * \file RecoParameters.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class RecoParameters
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __RECOPARAMETERS_H__
#define __RECOPARAMETERS_H__

#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {
  /**
     \class RecoParameters
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class RecoParameters : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    RecoParameters(const std::string name="RecoParameters") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~RecoParameters(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const Config_t &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);
    
  private:

  };
  
  /**
     \class larocv::RecoParametersFactory
     \brief A concrete factory class for larocv::RecoParameters
   */
  class RecoParametersFactory : public AlgoFactoryBase {
  public:
    /// ctor
    RecoParametersFactory() { AlgoFactory::get().add_factory("RecoParameters",this); }
    /// dtor
    ~RecoParametersFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new RecoParameters(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

