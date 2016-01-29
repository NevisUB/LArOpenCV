/**
 * \file ToyImageCluster.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ToyImageCluster
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __TOYIMAGECLUSTER_H__
#define __TOYIMAGECLUSTER_H__

#include "ImageClusterBase.h"

namespace larcv {
  /**
     \class ToyImageCluster
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ToyImageCluster : public larcv::ImageClusterBase {

  public:
    
    /// Default constructor
    ToyImageCluster(const std::string name="ToyImageCluster") : ImageClusterBase(name)
    {}
    
    /// Default destructor
    ~ToyImageCluster(){}

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
     \class larcv::ToyImageClusterFactory
     \brief A concrete factory class for larcv::ToyImageCluster
   */
  class ToyImageClusterFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    ToyImageClusterFactory() { ImageClusterFactory::get().add_factory("ToyImageCluster",this); }
    /// dtor
    ~ToyImageClusterFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new ToyImageCluster(instance_name); }
  };
  /// Global larcv::ToyImageClusterFactory to register ImageClusterFactory
  static ToyImageClusterFactory __global_ToyImageClusterFactory__;
}
#endif
/** @} */ // end of doxygen group 

