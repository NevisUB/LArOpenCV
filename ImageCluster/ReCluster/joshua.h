#ifndef __JOSHUA_H__
#define __JOSHUA_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {
  /**
     \class joshua
     @brief A simple clustering algorithm meant to serve for testing/example by Titus Cosgrove
  */
  
  class joshua : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    joshua(const std::string name="joshua") :
      ClusterAlgoBase(name)
      
    {  }
    
    /// Default destructor
    ~joshua(){}

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

  };
  
  /**
     \class larocv::joshuaFactory
     \brief A concrete factory class for larocv::joshua
   */
  class joshuaFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    joshuaFactory() { ClusterAlgoFactory::get().add_factory("joshua",this); }
    /// dtor
    ~joshuaFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new joshua(instance_name); }
  };
  /// Global larocv::joshuaFactory to register ClusterAlgoFactory
  static joshuaFactory __global_joshuaFactory__;
}
#endif
/** @} */ // end of doxygen group 

