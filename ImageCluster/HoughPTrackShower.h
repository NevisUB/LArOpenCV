/**
 * \file HoughPTrackShower.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class HoughPTrackShower
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __HOUGHPTRACKSHOWER_H__
#define __HOUGHPTRACKSHOWER_H__

#include "ImageClusterBase.h"
#include "TTree.h"

#include <array>

namespace larcv {
  /**
     \class HoughPTrackShower
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class HoughPTrackShower : public larcv::ImageClusterBase {
    
  public:
    
    /// Default constructor
    HoughPTrackShower(const std::string name="HoughPTrackShower")
      : ImageClusterBase(name),
	_outtree(nullptr),
	_pi(3.14159),
	
	_hough_rho       ( 20.0 ),
	_hough_threshold (  100 ),
	_hough_min_line_length ( 100 ),
	_hough_max_line_gap    (  60 )
    {}
    
    /// Default destructor
    ~HoughPTrackShower(){}

  private:
    TTree* _outtree;

  public:
    
    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile* _fout) {  _outtree->Write(); }

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::ContourArray_t _Process_(const larcv::ContourArray_t& clusters,
				    const ::cv::Mat& img,
				    larcv::ImageMeta& meta);



    std::vector<std::vector<std::array<int,4> > > _houghs_v;    

  private:

    double _pi;
    
    double _hough_rho;
    double _hough_theta;
    int    _hough_threshold;
    double _hough_min_line_length;
    double _hough_max_line_gap;

    
  };
  
  /**
     \class larcv::HoughPTrackShowerFactory
     \brief A concrete factory class for larcv::HoughPTrackShower
   */
  class HoughPTrackShowerFactory : public ImageClusterFactoryBase {
  public:
    /// ctor
    HoughPTrackShowerFactory() { ImageClusterFactory::get().add_factory("HoughPTrackShower",this); }
    /// dtor
    ~HoughPTrackShowerFactory() {}
    /// creation method
    ImageClusterBase* create(const std::string instance_name) { return new HoughPTrackShower(instance_name); }
  };
  /// Global larcv::HoughPTrackShowerFactory to register ImageClusterFactory
  static HoughPTrackShowerFactory __global_HoughPTrackShowerFactory__;
}
#endif
/** @} */ // end of doxygen group 

