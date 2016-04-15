/**
 * \file AttachedClusters.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class AttachedClusters
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __ATTACHEDCLUSTERS_H__
#define __ATTACHEDCLUSTERS_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class AttachedClusters
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class AttachedClusters : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    AttachedClusters(const std::string name="AttachedClusters") :
      ClusterAlgoBase(name)
      ,_maxDefectSize(99999)
    {}
    
    /// Default destructor
    ~AttachedClusters(){}

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

    int _maxDefectSize;
    
  };
  
  /**
     \class larcv::AttachedClustersFactory
     \brief A concrete factory class for larcv::AttachedClusters
   */
  class AttachedClustersFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    AttachedClustersFactory() { ClusterAlgoFactory::get().add_factory("AttachedClusters",this); }
    /// dtor
    ~AttachedClustersFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new AttachedClusters(instance_name); }
  };
  /// Global larcv::AttachedClustersFactory to register ClusterAlgoFactory
  static AttachedClustersFactory __global_AttachedClustersFactory__;
}
#endif
/** @} */ // end of doxygen group 

