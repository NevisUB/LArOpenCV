/**
 * \file FilterROI.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class FilterROI
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __FILTERROI_H__
#define __FILTERROI_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"
#include "TTree.h"

namespace larocv {
  /**
     \class FilterROI
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class FilterROI : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    FilterROI(const std::string name="FilterROI")
      : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~FilterROI(){}
    
    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile* _fout) { }
    
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
     \class larocv::FilterROIFactory
     \brief A concrete factory class for larocv::FilterROI
   */
  class FilterROIFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    FilterROIFactory() { ClusterAlgoFactory::get().add_factory("FilterROI",this); }
    /// dtor
    ~FilterROIFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new FilterROI(instance_name); }
  };
  /// Global larocv::FilterROIFactory to register ClusterAlgoFactory
  static FilterROIFactory __global_FilterROIFactory__;

}
#endif
/** @} */ // end of doxygen group 

