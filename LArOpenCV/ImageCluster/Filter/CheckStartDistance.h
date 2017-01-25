/**
 * \file CheckStartDistance.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class CheckStartDistance
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CHECKSTARTDISTANCE_H__
#define __CHECKSTARTDISTANCE_H__

#include "ClusterAlgoBase.h"
#include "AlgoFactory.h"

namespace larocv {
  /**
     \class CheckStartDistance
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class CheckStartDistance : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    CheckStartDistance(const std::string name="CheckStartDistance") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~CheckStartDistance(){}

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

    double _max_start_d;

    double _w;
    double _h;
    
  };
  
  /**
     \class larocv::CheckStartDistanceFactory
     \brief A concrete factory class for larocv::CheckStartDistance
   */
  class CheckStartDistanceFactory : public AlgoFactoryBase {
  public:
    /// ctor
    CheckStartDistanceFactory() { AlgoFactory::get().add_factory("CheckStartDistance",this); }
    /// dtor
    ~CheckStartDistanceFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new CheckStartDistance(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

