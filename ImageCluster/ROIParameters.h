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

namespace larcv {
  /**
     \class ROIParameters
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ROIParameters : public larcv::ClusterAlgoBase {
    
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
    larcv::Cluster2DArray_t _Process_(const larcv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larcv::ImageMeta& meta);
    
  private:

  };
  
  /**
     \class larcv::ROIParametersFactory
     \brief A concrete factory class for larcv::ROIParameters
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
  /// Global larcv::ROIParametersFactory to register ClusterAlgoFactory
  static ROIParametersFactory __global_ROIParametersFactory__;
}
#endif
/** @} */ // end of doxygen group 

