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

#include "ClusterAlgoBase.h"
#include "TTree.h"
#include "ClusterParams.h"
#include "ClusterAlgoFactory.h"
namespace larcv {
  /**
     \class TrackShower
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class TrackShower : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    TrackShower(const std::string name="TrackShower") : ClusterAlgoBase(name){}
    
    /// Default destructor
    ~TrackShower(){}

    std::vector<ClusterParams> _cparms_v;

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {} 

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larcv::Cluster2DArray_t _Process_(const larcv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larcv::ImageMeta& meta);

  private:

    float _area_cut ;
    float _ratio_separation ;
    int   _track_shower_sat ;

  };
  
  /**
     \class larcv::TrackShowerFactory
     \brief A concrete factory class for larcv::TrackShower
   */
  class TrackShowerFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    TrackShowerFactory() { ClusterAlgoFactory::get().add_factory("TrackShower",this); }
    /// dtor
    ~TrackShowerFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new TrackShower(instance_name); }
  };
  /// Global larcv::TrackShowerFactory to register ClusterAlgoFactory
  static TrackShowerFactory __global_TrackShowerFactory__;
}
#endif
/** @} */ // end of doxygen group 

