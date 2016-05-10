/**
 * \file InConeCluster.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class InConeCluster
 *
 * @author ariana Hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __INCONECLUSTER_H__
#define __INCONECLUSTER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class InConeCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class InConeCluster : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    InConeCluster(const std::string name="InConeCluster") :
      ClusterAlgoBase(name)
      , _cone_length(36.)
      , _cone_angle(7.)
      , _area_separation(700.)
      , _n_hits(25.)
    {}
    
    /// Default destructor
    ~InConeCluster(){}

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

    float _cone_length ;
    float _cone_angle ;
    float _area_separation ;
    float _n_hits;

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
     \class larcv::InConeClusterFactory
     \brief A concrete factory class for larcv::InConeCluster
   */
  class InConeClusterFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    InConeClusterFactory() { ClusterAlgoFactory::get().add_factory("InConeCluster",this); }
    /// dtor
    ~InConeClusterFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new InConeCluster(instance_name); }
  };
  /// Global larcv::InConeClusterFactory to register ClusterAlgoFactory
  static InConeClusterFactory __global_InConeClusterFactory__;
}
#endif
/** @} */ // end of doxygen group 

