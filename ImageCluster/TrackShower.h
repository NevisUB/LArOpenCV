/**
 * \file TrackShower.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class TrackShower
 *
 * @author ariana Hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __TRACKSHOWER_H__
#define __TRACKSHOWER_H__

#include "ImageClusterBase.h"
#include "TrackShowerVars.h"
namespace larcv {
  /**
     \class TrackShower
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class TrackShower : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    TrackShower(const std::string name="TrackShower") : ImageClusterBase(name)
    { _area_separation = 1850; _ratio_separation = 3; _track_shower_sat = 1; }
    
    /// Default destructor
    ~TrackShower(){}



    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);

  private:

    float _area_separation ;
    float _ratio_separation ;
    int   _track_shower_sat ;

  };
  
  /**
     \class larcv::TrackShowerFactory
     \brief A concrete factory class for larcv::TrackShower
   */
  class TrackShowerFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    TrackShowerFactory() { ImageClusterFactory::get().add_factory("TrackShower",this); }
    /// dtor
    ~TrackShowerFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new TrackShower(instance_name); }
    /// parameter construction
    AlgoVarsBase* vars() { return new TrackShowerVars; }
  };
  /// Global larcv::TrackShowerFactory to register ImageClusterFactory
  static TrackShowerFactory __global_TrackShowerFactory__;
}
#endif
/** @} */ // end of doxygen group 

