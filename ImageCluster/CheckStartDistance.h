/**
 * \file CheckStartDistance.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class CheckStartDistance
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CHECKSTARTDISTANCE_H__
#define __CHECKSTARTDISTANCE_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class CheckStartDistance
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class CheckStartDistance : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    CheckStartDistance(const std::string name="CheckStartDistance") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~CheckStartDistance(){}

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

    double _max_start_d;

    double _w;
    double _h;
    
  };
  
  /**
     \class larcv::CheckStartDistanceFactory
     \brief A concrete factory class for larcv::CheckStartDistance
   */
  class CheckStartDistanceFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    CheckStartDistanceFactory() { ClusterAlgoFactory::get().add_factory("CheckStartDistance",this); }
    /// dtor
    ~CheckStartDistanceFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new CheckStartDistance(instance_name); }
  };
  /// Global larcv::CheckStartDistanceFactory to register ClusterAlgoFactory
  static CheckStartDistanceFactory __global_CheckStartDistanceFactory__;
}
#endif
/** @} */ // end of doxygen group 

