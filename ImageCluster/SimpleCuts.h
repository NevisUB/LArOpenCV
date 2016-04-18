/**
 * \file SimpleCuts.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class SimpleCuts
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __SIMPLECUTS_H__
#define __SIMPLECUTS_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {
  /**
     \class SimpleCuts
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class SimpleCuts : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    SimpleCuts(const std::string name="SimpleCuts") :
      ClusterAlgoBase(name)
      ,_NHits(0)
    {}    
    
    /// Default destructor
    ~SimpleCuts(){}

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
    int _NHits;
    
  };
  
  /**
     \class larcv::SimpleCutsFactory
     \brief A concrete factory class for larcv::SimpleCuts
   */
  class SimpleCutsFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    SimpleCutsFactory() { ClusterAlgoFactory::get().add_factory("SimpleCuts",this); }
    /// dtor
    ~SimpleCutsFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new SimpleCuts(instance_name); }
  };
  /// Global larcv::SimpleCutsFactory to register ClusterAlgoFactory
  static SimpleCutsFactory __global_SimpleCutsFactory__;
}
#endif
/** @} */ // end of doxygen group 

