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

namespace larcv {
  /**
     \class ROIStart
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ROIStart : public larcv::ClusterAlgoBase {
    
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
    larcv::Cluster2DArray_t _Process_(const larcv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larcv::ImageMeta& meta);
    
  private:

  float _min_vertex_dist ;

  };
  
  /**
     \class larcv::ROIStartFactory
     \brief A concrete factory class for larcv::ROIStart
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
  /// Global larcv::ROIStartFactory to register ClusterAlgoFactory
  static ROIStartFactory __global_ROIStartFactory__;
}
#endif
/** @} */ // end of doxygen group 

