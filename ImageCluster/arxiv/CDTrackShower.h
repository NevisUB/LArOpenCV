/**
 * \file CDTrackShower.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class CDTrackShower
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CDTRACKSHOWER_H__
#define __CDTRACKSHOWER_H__

#include "ImageClusterBase.h"
#include "TTree.h"

namespace larcv {
  /**
     \class CDTrackShower
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class CDTrackShower : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    CDTrackShower(const std::string name="CDTrackShower")
      : ImageClusterBase(name),
	_outtree(nullptr)
    {}
    
    /// Default destructor
    ~CDTrackShower(){}

  private:
    TTree *_outtree;

  public:
    void Finalize(TFile* fout) { _outtree->Write(); }
    
    std::vector<std::vector<::cv::Vec4i> >  _defects_v;
    std::vector<std::vector<int> >          _hullpts_v;
    
    
  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);
    
  private:

    std::vector<double> _defects_dists;
    
  };
  
  /**
     \class larcv::CDTrackShowerFactory
     \brief A concrete factory class for larcv::CDTrackShower
  */
  class CDTrackShowerFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    CDTrackShowerFactory() { ImageClusterFactory::get().add_factory("CDTrackShower",this); }
    /// dtor
    ~CDTrackShowerFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new CDTrackShower(instance_name); }
  };
  /// Global larcv::CDTrackShowerFactory to register ImageClusterFactory
  static CDTrackShowerFactory __global_CDTrackShowerFactory__;
}
#endif
/** @} */ // end of doxygen group 

