/**
 * \file PolarMerge.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PolarMerge
 *
 * @author david caratelli
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __POLARMERGE_H__
#define __POLARMERGE_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class PolarMerge
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PolarMerge : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    PolarMerge(const std::string name="PolarMerge") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~PolarMerge(){}

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
    
  };
  
  /**
     \class larocv::PolarMergeFactory
     \brief A concrete factory class for larocv::PolarMerge
   */
  class PolarMergeFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    PolarMergeFactory() { ClusterAlgoFactory::get().add_factory("PolarMerge",this); }
    /// dtor
    ~PolarMergeFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new PolarMerge(instance_name); }
  };
  /// Global larocv::PolarMergeFactory to register ClusterAlgoFactory
  static PolarMergeFactory __global_PolarMergeFactory__;
}
#endif
/** @} */ // end of doxygen group 

