/**
 * \file TriangleClusterExt.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class TriangleClusterExt
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __FLASHLIGHTMERGE_H__
#define __FLASHLIGHTMERGE_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class FlashlightMerge
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class FlashlightMerge : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    FlashlightMerge(const std::string name="FlashlightMerge") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~FlashlightMerge(){}

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
    int _N;

    template <typename T> inline float sgn(T val) { return (T(0) < val) - (val < T(0)); }

  };
  
  /**
     \class larocv::FlashlightMergeFactory
     \brief A concrete factory class for larocv::FlashlightMerge
   */
  class FlashlightMergeFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    FlashlightMergeFactory() { ClusterAlgoFactory::get().add_factory("FlashlightMerge",this); }
    /// dtor
    ~FlashlightMergeFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new FlashlightMerge(instance_name); }
  };
  /// Global larocv::FlashlightMergeFactory to register ClusterAlgoFactory
  static FlashlightMergeFactory __global_FlashlightMergeFactory__;
}
#endif
/** @} */ // end of doxygen group 

