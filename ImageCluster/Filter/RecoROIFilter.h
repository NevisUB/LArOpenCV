/**
 * \file RecoROIFilter.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class RecoROIFilter
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __RECOROIFILTER_H__
#define __RECOROIFILTER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class RecoROIFilter
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class RecoROIFilter : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    RecoROIFilter(const std::string name="RecoROIFilter") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~RecoROIFilter(){}

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

    double _max_rad_length;
    int _maxclusters;
    int _minclusters;
    int _strict_hit_cut;
    
  };
  
  /**
     \class larocv::RecoROIFilterFactory
     \brief A concrete factory class for larocv::RecoROIFilter
   */
  class RecoROIFilterFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    RecoROIFilterFactory() { ClusterAlgoFactory::get().add_factory("RecoROIFilter",this); }
    /// dtor
    ~RecoROIFilterFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new RecoROIFilter(instance_name); }
  };
  /// Global larocv::RecoROIFilterFactory to register ClusterAlgoFactory
  static RecoROIFilterFactory __global_RecoROIFilterFactory__;
}
#endif
/** @} */ // end of doxygen group 

