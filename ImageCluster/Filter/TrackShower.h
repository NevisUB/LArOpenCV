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
//#include "ClusterParams.h"
#include "ClusterAlgoFactory.h"
namespace larocv {
  /**
     \class TrackShower
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class TrackShower : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    TrackShower(const std::string name="TrackShower") : ClusterAlgoBase(name){}
    
    /// Default destructor
    ~TrackShower(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {} 

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta, larocv::ROI& roi);

  private:

    float _area_cut ;
    float _ratio_separation ;
    int   _track_shower_sat ;

    int _step1;
    int _step2;

    float _switch_ratio; 
  };
  
  /**
     \class larocv::TrackShowerFactory
     \brief A concrete factory class for larocv::TrackShower
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
  /// Global larocv::TrackShowerFactory to register ClusterAlgoFactory
  static TrackShowerFactory __global_TrackShowerFactory__;
}
#endif
/** @} */ // end of doxygen group 

