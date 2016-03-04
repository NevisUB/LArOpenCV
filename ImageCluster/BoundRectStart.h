/**
 * \file BoundRectStart.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class BoundRectStart
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __BOUNDRECTSTART_H__
#define __BOUNDRECTSTART_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class BoundRectStart
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class BoundRectStart : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    BoundRectStart(const std::string name="BoundRectStart") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~BoundRectStart(){}

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
    int _nHitsCut;
    
  };
  
  /**
     \class larcv::BoundRectStartFactory
     \brief A concrete factory class for larcv::BoundRectStart
   */
  class BoundRectStartFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    BoundRectStartFactory() { ClusterAlgoFactory::get().add_factory("BoundRectStart",this); }
    /// dtor
    ~BoundRectStartFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new BoundRectStart(instance_name); }
  };
  /// Global larcv::BoundRectStartFactory to register ClusterAlgoFactory
  static BoundRectStartFactory __global_BoundRectStartFactory__;
}
#endif
/** @} */ // end of doxygen group 

