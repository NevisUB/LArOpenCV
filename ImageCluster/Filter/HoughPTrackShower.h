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

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"
#include "TTree.h"

namespace larocv {
  /**
     \class HoughPTrackShower
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class HoughPTrackShower : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    HoughPTrackShower(const std::string name="HoughPTrackShower")
      : ClusterAlgoBase(name)
      , _pi(3.14159)
      , _hough_rho(20.0)
      , _hough_threshold(100)
      , _hough_min_line_length(100)
      , _hough_max_line_gap(60)
    {}
    
    /// Default destructor
    ~HoughPTrackShower(){}
    
    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile* _fout) { }
    
  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				       const ::cv::Mat& img,
				       larocv::ImageMeta& meta, larocv::ROI& roi);

  private:

   double _pi;
    
    double _hough_rho;
    double _hough_theta;
    int    _hough_threshold;
    double _hough_min_line_length;
    double _hough_max_line_gap;
    
  };
  
  /**
     \class larocv::HoughPTrackShowerFactory
     \brief A concrete factory class for larocv::HoughPTrackShower
   */
  class HoughPTrackShowerFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    HoughPTrackShowerFactory() { ClusterAlgoFactory::get().add_factory("HoughPTrackShower",this); }
    /// dtor
    ~HoughPTrackShowerFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new HoughPTrackShower(instance_name); }
  };
  /// Global larocv::HoughPTrackShowerFactory to register ClusterAlgoFactory
  static HoughPTrackShowerFactory __global_HoughPTrackShowerFactory__;

}
#endif
/** @} */ // end of doxygen group 

