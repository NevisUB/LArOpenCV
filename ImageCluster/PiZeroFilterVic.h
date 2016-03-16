/**
 * \file PiZeroFilterVic.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PiZeroFilterVic
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PIZEROFILTERVIC_H__
#define __PIZEROFILTERVIC_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class PiZeroFilterVic
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PiZeroFilterVic : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    PiZeroFilterVic(const std::string name="PiZeroFilterVic") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~PiZeroFilterVic(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::Cluster2DArray_t _Process_(const larcv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larcv::ImageMeta& meta);


    int _nhits_cut;
    double _back_startPt;  
    double _min_area;
    double _max_rad_length; 
    double _width;
    double _height;


    bool  _attempt_merging;
    double _small_dot_prod;

  private:

    Point2D backprojectionpoint(Point2D point1, Point2D point2, Point2D point3, Point2D point4);
    double distance2D(Point2D point1, Point2D point2, double width, double height);
    Cluster2DArray_t merging(Cluster2DArray_t filtered);

  };
  
  /**
     \class larcv::PiZeroFilterVicFactory
     \brief A concrete factory class for larcv::PiZeroFilterVic
   */
  class PiZeroFilterVicFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    PiZeroFilterVicFactory() { ClusterAlgoFactory::get().add_factory("PiZeroFilterVic",this); }
    /// dtor
    ~PiZeroFilterVicFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new PiZeroFilterVic(instance_name); }
  };
  /// Global larcv::PiZeroFilterVicFactory to register ClusterAlgoFactory
  static PiZeroFilterVicFactory __global_PiZeroFilterVicFactory__;
}
#endif
/** @} */ // end of doxygen group 

