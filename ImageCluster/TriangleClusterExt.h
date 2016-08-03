/**
 * \file TriangleClusterExt.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class TriangleClusterExt
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __TRIANGLECLUSTEREXT_H__
#define __TRIANGLECLUSTEREXT_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class TriangleClusterExt
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class TriangleClusterExt : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    TriangleClusterExt(const std::string name="TriangleClusterExt") : ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    ~TriangleClusterExt(){}

    /// Finalize after (possily multiple) Process call. TFile may be used to write output.
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const ::fcllite::PSet &pset);

    /// Process method
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta);
    
  private:
    int _N;

    template <typename T> inline int sgn(T val) { return (T(0) < val) - (val < T(0)); }

  };
  
  /**
     \class larocv::TriangleClusterExtFactory
     \brief A concrete factory class for larocv::TriangleClusterExt
   */
  class TriangleClusterExtFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    TriangleClusterExtFactory() { ClusterAlgoFactory::get().add_factory("TriangleClusterExt",this); }
    /// dtor
    ~TriangleClusterExtFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new TriangleClusterExt(instance_name); }
  };
  /// Global larocv::TriangleClusterExtFactory to register ClusterAlgoFactory
  static TriangleClusterExtFactory __global_TriangleClusterExtFactory__;
}
#endif
/** @} */ // end of doxygen group 

