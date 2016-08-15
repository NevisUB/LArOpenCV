/**
 * \file PID.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class PID
 *
 * @author ariana Hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __PID_H__
#define __PID_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class PID
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class PID : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    PID(const std::string name="PID") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~PID(){}

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

  int _pdg;

  };
  
  /**
     \class larocv::PIDFactory
     \brief A concrete factory class for larocv::PID
   */
  class PIDFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    PIDFactory() { ClusterAlgoFactory::get().add_factory("PID",this); }
    /// dtor
    ~PIDFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new PID(instance_name); }
  };
  /// Global larocv::PIDFactory to register ClusterAlgoFactory
  static PIDFactory __global_PIDFactory__;
}
#endif
/** @} */ // end of doxygen group 

