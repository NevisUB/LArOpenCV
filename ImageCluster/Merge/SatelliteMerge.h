/**
 * \file SatelliteMerge.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class SatelliteMerge
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __SATELLITEMERGE_H__
#define __SATELLITEMERGE_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class SatelliteMerge
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class SatelliteMerge : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    SatelliteMerge(const std::string name="SatelliteMerge") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~SatelliteMerge(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

    std::vector<Contour_t> _secret_initial_sats;
    
  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);
    
  private:

    double _area_separation;
    double _min_sat_dist;
    double _min_shower_dist;
    double _density;         

    void _combine_two_contours(const larocv::Contour_t& c1, const larocv::Contour_t& c2, larocv::Contour_t& c3);
    double _pt_distance       (const larocv::Contour_t& c1, const larocv::Contour_t& c2);
    
  };
  
  /**
     \class larocv::SatelliteMergeFactory
     \brief A concrete factory class for larocv::SatelliteMerge
   */
  class SatelliteMergeFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    SatelliteMergeFactory() { ClusterAlgoFactory::get().add_factory("SatelliteMerge",this); }
    /// dtor
    ~SatelliteMergeFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new SatelliteMerge(instance_name); }
  };
  /// Global larocv::SatelliteMergeFactory to register ClusterAlgoFactory
  static SatelliteMergeFactory __global_SatelliteMergeFactory__;
}
#endif
/** @} */ // end of doxygen group 

