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

#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

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
    void _Configure_(const Config_t &pset);

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
  class ROIParametersFactory : public AlgoFactoryBase {
  public:
    /// ctor
    ROIParametersFactory() { AlgoFactory::get().add_factory("ROIParameters",this); }
    /// dtor
    ~ROIParametersFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new ROIParameters(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

