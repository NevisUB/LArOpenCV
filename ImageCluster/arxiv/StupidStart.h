/**
 * \file StupidStart.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class StupidStart
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __STUPIDSTART_H__
#define __STUPIDSTART_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class StupidStart
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class StupidStart : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    StupidStart(const std::string name="StupidStart") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~StupidStart(){}

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
     \class larcv::StupidStartFactory
     \brief A concrete factory class for larcv::StupidStart
   */
  class StupidStartFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    StupidStartFactory() { ClusterAlgoFactory::get().add_factory("StupidStart",this); }
    /// dtor
    ~StupidStartFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new StupidStart(instance_name); }
  };
  /// Global larcv::StupidStartFactory to register ClusterAlgoFactory
  static StupidStartFactory __global_StupidStartFactory__;
}
#endif
/** @} */ // end of doxygen group 

