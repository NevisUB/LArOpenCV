/**
 * \file EmptyImageCluster.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class EmptyImageCluster
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __EMPTYIMAGECLUSTER_H__
#define __EMPTYIMAGECLUSTER_H__

#include "ImageClusterBase.h"

namespace larcv {
  /**
     \class EmptyImageCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class EmptyImageCluster : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    EmptyImageCluster(const std::string name="EmptyImageCluster") : ImageClusterBase(name)
    {}
    
    /// Default destructor
    ~EmptyImageCluster(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    
  private:

    std::vector<double> _canny_params;

  };

  /**
     \class larcv::EmptyImageClusterFactory
     \brief A concrete factory class for larcv::EmptyImageCluster
   */
  class EmptyImageClusterFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    EmptyImageClusterFactory() { ImageClusterFactory::get().add_factory("EmptyImageCluster",this); }
    /// dtor
    ~EmptyImageClusterFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new EmptyImageCluster(instance_name); }
  };
  /// Global larcv::EmptyImageClusterFactory to register ImageClusterFactory
  static EmptyImageClusterFactory __global_EmptyImageClusterFactory__;
}
#endif
/** @} */ // end of doxygen group 

