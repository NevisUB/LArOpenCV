/**
 * \file ROIStart.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ROIStart
 *
 * @author ariana hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __SIMPLEENDPOINT_H__
#define __SIMPLEENDPOINT_H__

#include "LArOpenCV/ImageCluster/Base/ClusterAlgoBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {
  /**
     \class SimpleEndPoint
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class SimpleEndPoint : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    SimpleEndPoint(const std::string name="SimpleEndPoint") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~SimpleEndPoint(){}

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

    float dist(const ::cv::Point& pt1, const Point2D& pt2);
  };
  
  /**
     \class larocv::SimpleEndPointFactory
     \brief A concrete factory class for larocv::SimpleEndPoint
  */
  class SimpleEndPointFactory : public AlgoFactoryBase {
  public:
    /// ctor
    SimpleEndPointFactory() { AlgoFactory::get().add_factory("SimpleEndPoint",this); }
    /// dtor
    ~SimpleEndPointFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new SimpleEndPoint(instance_name); }
  };
  /// Global larocv::SimpleEndPointFactory to register AlgoFactory
  static SimpleEndPointFactory __global_SimpleEndPointFactory__;
}
#endif
/** @} */ // end of doxygen group 

