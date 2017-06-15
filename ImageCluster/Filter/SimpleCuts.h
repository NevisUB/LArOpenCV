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

namespace larocv {
  /**
     \class SimpleCuts
     @brief A simple clustering algorithm meant to serve for testing/example by Kazu
  */
  class SimpleCuts : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    SimpleCuts(const std::string name="SimpleCuts") :
      ClusterAlgoBase(name)
      ,_min_hits(-1)
      ,_min_area(-1)
      ,_min_perimeter(-1)
      ,_min_charge(-1)
      ,_min_length(-1)
      ,_min_width(-1)
      ,_pool(false)
      ,_min_pool_hits(-1)
      
    {}    
    
    /// Default destructor
    ~SimpleCuts(){}

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

    int _min_hits;
    float _min_area;  
    float _min_perimeter;
    int _min_charge;
    float _min_length;
    float _min_width;

    // Add option to use pool-correct n hits
    bool _pool ;
    int _min_pool_hits ;

  };
  
  /**
     \class larocv::SimpleCutsFactory
     \brief A concrete factory class for larocv::SimpleCuts
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
  /// Global larocv::SimpleCutsFactory to register ClusterAlgoFactory
  static SimpleCutsFactory __global_SimpleCutsFactory__;
}
#endif
/** @} */ // end of doxygen group 

