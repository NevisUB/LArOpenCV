/**
 * \file ClusDeadWireOverlap.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ClusDeadWireOverlap
 *
 * @author ariana hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CLUSDEADWIREOVERLAP_H__
#define __CLUSDEADWIREOVERLAP_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class ClusDeadWireOverlap
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class ClusDeadWireOverlap : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    ClusDeadWireOverlap(const std::string name="ClusDeadWireOverlap") : ClusterAlgoBase(name)
    { _wires_v.resize(3); }
    
    /// Default destructor
    ~ClusDeadWireOverlap(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi);
    
  private:

  std::vector<std::vector<std::pair<int,int> > > _wires_v;
  std::vector<std::pair<int,int> > LoadWires(ImageMeta& meta);

  };
  
  /**
     \class larocv::ClusDeadWireOverlapFactory
     \brief A concrete factory class for larocv::ClusDeadWireOverlap
   */
  class ClusDeadWireOverlapFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    ClusDeadWireOverlapFactory() { ClusterAlgoFactory::get().add_factory("ClusDeadWireOverlap",this); }
    /// dtor
    ~ClusDeadWireOverlapFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new ClusDeadWireOverlap(instance_name); }
  };
  /// Global larocv::ClusDeadWireOverlapFactory to register ClusterAlgoFactory
  static ClusDeadWireOverlapFactory __global_ClusDeadWireOverlapFactory__;
}
#endif
/** @} */ // end of doxygen group 

