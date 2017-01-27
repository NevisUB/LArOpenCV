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

#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {
  /**
     \class InConeCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class InConeCluster : public larocv::ClusterAlgoBase {
    
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
    void _Configure_(const Config_t &pset);

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
     \class larocv::InConeClusterFactory
     \brief A concrete factory class for larocv::InConeCluster
   */
  class InConeClusterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    InConeClusterFactory() { AlgoFactory::get().add_factory("InConeCluster",this); }
    /// dtor
    ~InConeClusterFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new InConeCluster(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

