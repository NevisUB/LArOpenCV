/**
 * \file CheckWires.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class CheckWires
 *
 * @author ariana Hackenburg 
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __CHECKWIRES_H__
#define __CHECKWIRES_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class CheckWires
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class CheckWires : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    CheckWires(const std::string name="CheckWires") : ClusterAlgoBase(name)
    { _wires_v.resize(3); }
    
    /// Default destructor
    ~CheckWires(){}

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
     \class larocv::CheckWiresFactory
     \brief A concrete factory class for larocv::CheckWires
   */
  class CheckWiresFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    CheckWiresFactory() { ClusterAlgoFactory::get().add_factory("CheckWires",this); }
    /// dtor
    ~CheckWiresFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new CheckWires(instance_name); }
  };
  /// Global larocv::CheckWiresFactory to register ClusterAlgoFactory
  static CheckWiresFactory __global_CheckWiresFactory__;
}
#endif
/** @} */ // end of doxygen group 

