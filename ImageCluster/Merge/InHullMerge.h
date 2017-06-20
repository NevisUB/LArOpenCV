/**
 * \file InHullMerge.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class InHullMerge
 *
 * @author ariana Hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __INHULLMERGE_H__
#define __INHULLMERGE_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class InHullMerge
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class InHullMerge : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    InHullMerge(const std::string name="InHullMerge") :
      ClusterAlgoBase(name)
      , _cone_length(36.)
      , _cone_angle(7.)
      , _area_separation(700.)
      , _n_hits(25.)
    {}
    
    /// Default destructor
    ~InHullMerge(){}

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

    float _cone_length ;
    float _cone_angle ;
    float _area_separation ;
    float _n_hits;

    void _combine_two_contours(const larocv::Contour_t& c1, const larocv::Contour_t& c2, larocv::Contour_t& c3);
    void _combine_two_contours(const larocv::Contour_t& c1, const larocv::Contour_t& c2, larocv::Contour_t& c3, const int & offset);

    /// We have issues combining contours as this introduces twists. If we don't fix the twists, later down the
    /// line we have problems with associating hits to contours (twists may make it seem like hits are outside
    /// contours).  The solution: find the closest distance between one of shower contour points and satellite 
    /// COM. Store these shower contour locations in a map in descending order.  We will add the contours from
    /// the end of the list-- adding to the end of the list in order of contour point-proximity to satellite
    /// seems to fix this problem most of the time. In this function, I store these indices.
    void _order_sats(larocv::Cluster2D& c1, larocv::Cluster2D & c2, std::map<float,larocv::Contour_t> & merge_us, const ::cv::Point & COM );

  };
  
  /**
     \class larocv::InHullMergeFactory
     \brief A concrete factory class for larocv::InHullMerge
   */
  class InHullMergeFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    InHullMergeFactory() { ClusterAlgoFactory::get().add_factory("InHullMerge",this); }
    /// dtor
    ~InHullMergeFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new InHullMerge(instance_name); }
  };
  /// Global larocv::InHullMergeFactory to register ClusterAlgoFactory
  static InHullMergeFactory __global_InHullMergeFactory__;
}
#endif
/** @} */ // end of doxygen group 

