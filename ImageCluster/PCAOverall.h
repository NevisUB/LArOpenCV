/**
 * \file PCAOverall.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PCAOverall
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PCAOVERALL_H__
#define __PCAOVERALL_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class PCAOverall
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PCAOverall : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    PCAOverall(const std::string name="PCAOverall") :
      ClusterAlgoBase(name),
      _nMinInsideHits(0)
    {}
    
    /// Default destructor
    ~PCAOverall(){}

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

    int _nMinInsideHits;
    
  };
  
  /**
     \class larcv::PCAOverallFactory
     \brief A concrete factory class for larcv::PCAOverall
   */
  class PCAOverallFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    PCAOverallFactory() { ClusterAlgoFactory::get().add_factory("PCAOverall",this); }
    /// dtor
    ~PCAOverallFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new PCAOverall(instance_name); }
  };
  /// Global larcv::PCAOverallFactory to register ClusterAlgoFactory
  static PCAOverallFactory __global_PCAOverallFactory__;
}
#endif
/** @} */ // end of doxygen group 

