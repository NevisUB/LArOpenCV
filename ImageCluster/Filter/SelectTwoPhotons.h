/**
 * \file SelectTwoPhotons.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class SelectTwoPhotons
 *
 * @author David Caratelli
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __SELECTTWOPHOTONS_H__
#define __SELECTTWOPHOTONS_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class SelectTwoPhotons
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class SelectTwoPhotons : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    SelectTwoPhotons(const std::string name="SelectTwoPhotons") :
      ClusterAlgoBase(name)
    {}    
    
    /// Default destructor
    ~SelectTwoPhotons(){}

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

  };
  
  /**
     \class larocv::SelectTwoPhotonsFactory
     \brief A concrete factory class for larocv::SelectTwoPhotons
   */
  class SelectTwoPhotonsFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    SelectTwoPhotonsFactory() { ClusterAlgoFactory::get().add_factory("SelectTwoPhotons",this); }
    /// dtor
    ~SelectTwoPhotonsFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new SelectTwoPhotons(instance_name); }
  };
  /// Global larocv::SelectTwoPhotonsFactory to register ClusterAlgoFactory
  static SelectTwoPhotonsFactory __global_SelectTwoPhotonsFactory__;
}
#endif
/** @} */ // end of doxygen group 

