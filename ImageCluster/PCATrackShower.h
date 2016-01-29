/**
 * \file PCATrackShower.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PCATrackShower
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PCATRACKSHOWER_H__
#define __PCATRACKSHOWER_H__

#include "ImageClusterBase.h"

namespace larcv {
  /**
     \class PCATrackShower
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PCATrackShower : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    PCATrackShower(const std::string name="PCATrackShower") : ImageClusterBase(name)
    {}
    
    /// Default destructor
    ~PCATrackShower(){}

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
     \class larcv::PCATrackShowerFactory
     \brief A concrete factory class for larcv::PCATrackShower
   */
  class PCATrackShowerFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    PCATrackShowerFactory() { ImageClusterFactory::get().add_factory("PCATrackShower",this); }
    /// dtor
    ~PCATrackShowerFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new PCATrackShower(instance_name); }
  };
  /// Global larcv::PCATrackShowerFactory to register ImageClusterFactory
  static PCATrackShowerFactory __global_PCATrackShowerFactory__;
}
#endif
/** @} */ // end of doxygen group 

