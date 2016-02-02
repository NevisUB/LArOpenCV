/**
 * \file SatelliteMerge.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class SatelliteMerge
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __SATELLITEMERGE_H__
#define __SATELLITEMERGE_H__

#include "ImageClusterBase.h"

namespace larcv {
  /**
     \class SatelliteMerge
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class SatelliteMerge : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    SatelliteMerge(const std::string name="SatelliteMerge") : ImageClusterBase(name)
    {}
    
    /// Default destructor
    ~SatelliteMerge(){}

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


  };
  
  /**
     \class larcv::SatelliteMergeFactory
     \brief A concrete factory class for larcv::SatelliteMerge
   */
  class SatelliteMergeFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    SatelliteMergeFactory() { ImageClusterFactory::get().add_factory("SatelliteMerge",this); }
    /// dtor
    ~SatelliteMergeFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new SatelliteMerge(instance_name); }
  };
  /// Global larcv::SatelliteMergeFactory to register ImageClusterFactory
  static SatelliteMergeFactory __global_SatelliteMergeFactory__;
}
#endif
/** @} */ // end of doxygen group 

