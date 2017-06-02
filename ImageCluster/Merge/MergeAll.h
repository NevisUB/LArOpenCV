/**
 * \file MergeAll.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class MergeAll
 *
 * @author ariana Hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __MERGEALL_H__
#define __MERGEALL_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class MergeAll
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class MergeAll : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    MergeAll(const std::string name="MergeAll") :
      ClusterAlgoBase(name)
      , _cone_length(36.)
      , _cone_angle(7.)
      , _area_separation(700.)
      , _n_hits(25.)
    {}
    
    /// Default destructor
    ~MergeAll(){}

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
     \class larocv::MergeAllFactory
     \brief A concrete factory class for larocv::MergeAll
   */
  class MergeAllFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    MergeAllFactory() { ClusterAlgoFactory::get().add_factory("MergeAll",this); }
    /// dtor
    ~MergeAllFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new MergeAll(instance_name); }
  };
  /// Global larocv::MergeAllFactory to register ClusterAlgoFactory
  static MergeAllFactory __global_MergeAllFactory__;
}
#endif
/** @} */ // end of doxygen group 

