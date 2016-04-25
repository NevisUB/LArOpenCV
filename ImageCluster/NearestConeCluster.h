/**
 * \file NearestConeCluster.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class NearestConeCluster
 *
 * @author ariana Hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __NEARESTCONECLUSTER_H__
#define __NEARESTCONECLUSTER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class NearestConeCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class NearestConeCluster : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    NearestConeCluster(const std::string name="NearestConeCluster") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~NearestConeCluster(){}

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

    void _combine_two_contours(const larcv::Contour_t& c1, const larcv::Contour_t& c2, larcv::Contour_t& c3);
    void _combine_two_contours(const larcv::Contour_t& c1, const larcv::Contour_t& c2, larcv::Contour_t& c3, const int & offset);

    /// We have issues combining contours as this introduces twists. If we don't fix the twists, later down the
    /// line we have problems with associating hits to contours (twists may make it seem like hits are outside
    /// contours).  The solution: find the closest distance between one of shower contour points and satellite 
    /// COM. Store these shower contour locations in a map in descending order.  We will add the contours from
    /// the end of the list-- adding to the end of the list in order of contour point-proximity to satellite
    /// seems to fix this problem most of the time. In this function, I store these indices.
    void _order_sats(larcv::Cluster2D& c1, larcv::Cluster2D & c2, std::map<float,larcv::Contour_t> & merge_us, const ::cv::Point & COM );


};
  
  /**
     \class larcv::NearestConeClusterFactory
     \brief A concrete factory class for larcv::NearestConeCluster
   */
  class NearestConeClusterFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    NearestConeClusterFactory() { ClusterAlgoFactory::get().add_factory("NearestConeCluster",this); }
    /// dtor
    ~NearestConeClusterFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new NearestConeCluster(instance_name); }
  };
  /// Global larcv::NearestConeClusterFactory to register ClusterAlgoFactory
  static NearestConeClusterFactory __global_NearestConeClusterFactory__;
}
#endif
/** @} */ // end of doxygen group 

