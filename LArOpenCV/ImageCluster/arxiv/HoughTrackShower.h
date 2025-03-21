/**
 * \file HoughTrackShower.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class HoughTrackShower
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __HOUGHTRACKSHOWER_H__
#define __HOUGHTRACKSHOWER_H__

#include "ImageClusterBase.h"
#include "TTree.h"

namespace larcv {
  /**
     \class HoughTrackShower
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class HoughTrackShower : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    HoughTrackShower(const std::string name="HoughTrackShower")
      : ImageClusterBase(name),
	_outtree(nullptr)
    {}
    
    /// Default destructor
    ~HoughTrackShower(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile* _fout) {  _outtree->Write(); }

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
     \class larcv::HoughTrackShowerFactory
     \brief A concrete factory class for larcv::HoughTrackShower
   */
  class HoughTrackShowerFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    HoughTrackShowerFactory() { ImageClusterFactory::get().add_factory("HoughTrackShower",this); }
    /// dtor
    ~HoughTrackShowerFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new HoughTrackShower(instance_name); }
  };
  /// Global larcv::HoughTrackShowerFactory to register ImageClusterFactory
  static HoughTrackShowerFactory __global_HoughTrackShowerFactory__;
}
#endif
/** @} */ // end of doxygen group 

