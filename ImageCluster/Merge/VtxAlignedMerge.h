/**
 * \file VtxAlignedMerge.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class VtxAlignedMerge
 *
 * @author david caratelli
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __VTXALIGNEDMERGE_H__
#define __VTXALIGNEDMERGE_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class VtxAlignedMerge
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class VtxAlignedMerge : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    VtxAlignedMerge(const std::string name="VtxAlignedMerge") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~VtxAlignedMerge(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    bool _debug;

    // algorithm-specific parameters

    // maximum angle between subclusters of the same shower for merging
    double _max_angle_diff_merge;

    // minimum opening angle between two main showers to count
    // as separate photons
    double _min_gammagamma_oangle;

    // iteratively merge untill no clusters have been merged
    bool _merge_till_converge;
    
    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);
    
  private:
    
  };
  
  /**
     \class larocv::VtxAlignedMergeFactory
     \brief A concrete factory class for larocv::VtxAlignedMerge
   */
  class VtxAlignedMergeFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    VtxAlignedMergeFactory() { ClusterAlgoFactory::get().add_factory("VtxAlignedMerge",this); }
    /// dtor
    ~VtxAlignedMergeFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new VtxAlignedMerge(instance_name); }
  };
  /// Global larocv::VtxAlignedMergeFactory to register ClusterAlgoFactory
  static VtxAlignedMergeFactory __global_VtxAlignedMergeFactory__;
}
#endif
/** @} */ // end of doxygen group 

