/**
 * \file PiZeroFilter.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PiZeroFilter
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PIZEROFILTER_H__
#define __PIZEROFILTER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class PiZeroFilter
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PiZeroFilter : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    PiZeroFilter(const std::string name="PiZeroFilter") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~PiZeroFilter(){}

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


  private:

    Point2D backprojectionpoint(Point2D point1, Point2D point2, Point2D point3, Point2D point4);
    double distance2D(Point2D point1, Point2D point2, double width, double height);

  };
  
  /**
     \class larcv::PiZeroFilterFactory
     \brief A concrete factory class for larcv::PiZeroFilter
   */
  class PiZeroFilterFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    PiZeroFilterFactory() { ClusterAlgoFactory::get().add_factory("PiZeroFilter",this); }
    /// dtor
    ~PiZeroFilterFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new PiZeroFilter(instance_name); }
  };
  /// Global larcv::PiZeroFilterFactory to register ClusterAlgoFactory
  static PiZeroFilterFactory __global_PiZeroFilterFactory__;
}
#endif
/** @} */ // end of doxygen group 

