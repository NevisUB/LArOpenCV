/**
 * \file InertiaTrackShower.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class InertiaTrackShower
 *
 * @author vic
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __INERTIATRACKSHOWER_H__
#define __INERTIATRACKSHOWER_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class InertiaTrackShower
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class InertiaTrackShower : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    InertiaTrackShower(const std::string name="InertiaTrackShower")
      : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~InertiaTrackShower(){}
    
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
    
  };
  
  /**
     \class larocv::InertiaTrackShowerFactory
     \brief A concrete factory class for larocv::InertiaTrackShower
   */
  class InertiaTrackShowerFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    InertiaTrackShowerFactory() { ClusterAlgoFactory::get().add_factory("InertiaTrackShower",this); }
    /// dtor
    ~InertiaTrackShowerFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new InertiaTrackShower(instance_name); }
  };
  /// Global larocv::InertiaTrackShowerFactory to register ClusterAlgoFactory
  static InertiaTrackShowerFactory __global_InertiaTrackShowerFactory__;

}
#endif
/** @} */ // end of doxygen group 

