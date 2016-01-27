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

  class EmptyImageClusterFactory : public ImageClusterFactoryBase {
  public:
    EmptyImageClusterFactory() { ImageClusterFactory::get().add_factory("EmptyImageCluster",this); }
    ~EmptyImageClusterFactory() {}
    ImageClusterBase* create() { return new EmptyImageCluster; }
  };

  static EmptyImageClusterFactory __global_EmptyImageClusterFactory__;
}
#endif
/** @} */ // end of doxygen group 

