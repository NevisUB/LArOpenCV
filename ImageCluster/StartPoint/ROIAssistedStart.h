/**
 * \file ROIAssistedStart.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ROIAssistedStart
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __ROIASSISTEDSTART_H__
#define __ROIASSISTEDSTART_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class ROIAssistedStart
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ROIAssistedStart : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    ROIAssistedStart(const std::string name="ROIAssistedStart") :
      ClusterAlgoBase(name)
      , _deg2rad ( 3.14159 / 180.0 )
    {}
    
    /// Default destructor
    ~ROIAssistedStart(){}

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

    float _deg2rad;

    float _padx;
    float _pady;
    
  };
  
  /**
     \class larocv::ROIAssistedStartFactory
     \brief A concrete factory class for larocv::ROIAssistedStart
   */
  class ROIAssistedStartFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    ROIAssistedStartFactory() { ClusterAlgoFactory::get().add_factory("ROIAssistedStart",this); }
    /// dtor
    ~ROIAssistedStartFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new ROIAssistedStart(instance_name); }
  };
  /// Global larocv::ROIAssistedStartFactory to register ClusterAlgoFactory
  static ROIAssistedStartFactory __global_ROIAssistedStartFactory__;
}
#endif
/** @} */ // end of doxygen group 

