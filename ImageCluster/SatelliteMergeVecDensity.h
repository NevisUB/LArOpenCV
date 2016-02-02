/**
 * \file SatelliteMergeVecDensity.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class SatelliteMergeVecDensity
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __SATELLITEMERGEVECDENSITY_H__
#define __SATELLITEMERGEVECDENSITY_H__

#include "ImageClusterBase.h"
#include "TTree.h"

namespace larcv {
  /**
     \class SatelliteMergeVecDensity
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class SatelliteMergeVecDensity : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    SatelliteMergeVecDensity(const std::string name="SatelliteMergeVecDensity") :
      ImageClusterBase(name),
      _outtree(nullptr)
    {}
    
    /// Default destructor
    ~SatelliteMergeVecDensity(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile* fout) { _outtree->Write(); }

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    
  private:

    TTree* _outtree;

  };
  
  /**
     \class larcv::SatelliteMergeVecDensityFactory
     \brief A concrete factory class for larcv::SatelliteMergeVecDensity
   */
  class SatelliteMergeVecDensityFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    SatelliteMergeVecDensityFactory() { ImageClusterFactory::get().add_factory("SatelliteMergeVecDensity",this); }
    /// dtor
    ~SatelliteMergeVecDensityFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new SatelliteMergeVecDensity(instance_name); }
  };
  /// Global larcv::SatelliteMergeVecDensityFactory to register ImageClusterFactory
  static SatelliteMergeVecDensityFactory __global_SatelliteMergeVecDensityFactory__;
}
#endif
/** @} */ // end of doxygen group 

