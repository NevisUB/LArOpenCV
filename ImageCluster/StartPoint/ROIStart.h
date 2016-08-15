/**
 * \file ROIStart.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ROIStart
 *
 * @author ariana hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __ROISTART_H__
#define __ROISTART_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class ROIStart
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ROIStart : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    ROIStart(const std::string name="ROIStart") : ClusterAlgoBase(name), _min_vertex_dist(0.)
    
    {}
    
    /// Default destructor
    ~ROIStart(){}

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

  float _min_vertex_dist ;

  };
  
  /**
     \class larocv::ROIStartFactory
     \brief A concrete factory class for larocv::ROIStart
   */
  class ROIStartFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    ROIStartFactory() { ClusterAlgoFactory::get().add_factory("ROIStart",this); }
    /// dtor
    ~ROIStartFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new ROIStart(instance_name); }
  };
  /// Global larocv::ROIStartFactory to register ClusterAlgoFactory
  static ROIStartFactory __global_ROIStartFactory__;
}
#endif
/** @} */ // end of doxygen group 

