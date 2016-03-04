/**
 * \file MinNClusters.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class MinNClusters
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __MINNCLUSTERS_H__
#define __MINNCLUSTERS_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class MinNClusters
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class MinNClusters : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    MinNClusters(const std::string name="MinNClusters") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~MinNClusters(){}

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
     \class larcv::MinNClustersFactory
     \brief A concrete factory class for larcv::MinNClusters
   */
  class MinNClustersFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    MinNClustersFactory() { ClusterAlgoFactory::get().add_factory("MinNClusters",this); }
    /// dtor
    ~MinNClustersFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new MinNClusters(instance_name); }
  };
  /// Global larcv::MinNClustersFactory to register ClusterAlgoFactory
  static MinNClustersFactory __global_MinNClustersFactory__;
}
#endif
/** @} */ // end of doxygen group 

