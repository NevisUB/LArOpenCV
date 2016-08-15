/**
 * \file ROIParameters.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ROIParameters
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __ROIPARAMETERS_H__
#define __ROIPARAMETERS_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class ROIParameters
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ROIParameters : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    ROIParameters(const std::string name="ROIParameters") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~ROIParameters(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);
    
  private:

  };
  
  /**
     \class larocv::ROIParametersFactory
     \brief A concrete factory class for larocv::ROIParameters
   */
  class ROIParametersFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    ROIParametersFactory() { ClusterAlgoFactory::get().add_factory("ROIParameters",this); }
    /// dtor
    ~ROIParametersFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new ROIParameters(instance_name); }
  };
  /// Global larocv::ROIParametersFactory to register ClusterAlgoFactory
  static ROIParametersFactory __global_ROIParametersFactory__;
}
#endif
/** @} */ // end of doxygen group 

