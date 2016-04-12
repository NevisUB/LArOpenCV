/**
 * \file RadLengthFilter.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class RadLengthFilter
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __RADLENGTHFILTER_H__
#define __RADLENGTHFILTER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class RadLengthFilter
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class RadLengthFilter : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    RadLengthFilter(const std::string name="RadLengthFilter") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~RadLengthFilter(){}

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
     \class larcv::RadLengthFilterFactory
     \brief A concrete factory class for larcv::RadLengthFilter
   */
  class RadLengthFilterFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    RadLengthFilterFactory() { ClusterAlgoFactory::get().add_factory("RadLengthFilter",this); }
    /// dtor
    ~RadLengthFilterFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new RadLengthFilter(instance_name); }
  };
  /// Global larcv::RadLengthFilterFactory to register ClusterAlgoFactory
  static RadLengthFilterFactory __global_RadLengthFilterFactory__;
}
#endif
/** @} */ // end of doxygen group 

