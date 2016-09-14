/**
 * \file CheckAlignment.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class CheckAlignment
 *
 * @author ariana hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CHECKALIGNMENT_H__
#define __CHECKALIGNMENT_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class CheckAlignment
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class CheckAlignment : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    CheckAlignment(const std::string name="CheckAlignment") : ClusterAlgoBase(name), _ratio_cut(0.6)
    {}
    
    /// Default destructor
    ~CheckAlignment(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi);
    
  private:

  double _ratio_cut ;

  };
  
  /**
     \class larocv::CheckAlignmentFactory
     \brief A concrete factory class for larocv::CheckAlignment
   */
  class CheckAlignmentFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    CheckAlignmentFactory() { ClusterAlgoFactory::get().add_factory("CheckAlignment",this); }
    /// dtor
    ~CheckAlignmentFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new CheckAlignment(instance_name); }
  };
  /// Global larocv::CheckAlignmentFactory to register ClusterAlgoFactory
  static CheckAlignmentFactory __global_CheckAlignmentFactory__;
}
#endif
/** @} */ // end of doxygen group 

