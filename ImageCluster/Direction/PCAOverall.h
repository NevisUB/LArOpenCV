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

namespace larocv {
  /**
     \class PCAOverall
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PCAOverall : public larocv::ClusterAlgoBase {
    
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
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta);
    
  private:

    int _nMinInsideHits;
    
  };
  
  /**
     \class larocv::PCAOverallFactory
     \brief A concrete factory class for larocv::PCAOverall
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
  /// Global larocv::PCAOverallFactory to register ClusterAlgoFactory
  static PCAOverallFactory __global_PCAOverallFactory__;
}
#endif
/** @} */ // end of doxygen group 

