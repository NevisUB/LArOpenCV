/**
 * \file UpdateMeta.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class UpdateMeta
 *
 * @author ariana hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __UPDATEMETA_H__
#define __UPDATEMETA_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class UpdateMeta
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class UpdateMeta : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    UpdateMeta(const std::string name="UpdateMeta") :
      ClusterAlgoBase(name)
    {}    
    
    /// Default destructor
    ~UpdateMeta(){}

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
     \class larocv::UpdateMetaFactory
     \brief A concrete factory class for larocv::UpdateMeta
   */
  class UpdateMetaFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    UpdateMetaFactory() { ClusterAlgoFactory::get().add_factory("UpdateMeta",this); }
    /// dtor
    ~UpdateMetaFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new UpdateMeta(instance_name); }
  };
  /// Global larocv::UpdateMetaFactory to register ClusterAlgoFactory
  static UpdateMetaFactory __global_UpdateMetaFactory__;
}
#endif
/** @} */ // end of doxygen group 

