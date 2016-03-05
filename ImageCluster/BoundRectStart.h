#ifndef __BOUNDRECTSTART_H__
#define __BOUNDRECTSTART_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larcv {

  
  class BoundRectStart : public larcv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    BoundRectStart(const std::string name="BoundRectStart") :
      ClusterAlgoBase(name),
      _deg2rad ( 3.14159 / 180.0 )
    {}
    
    /// Default destructor
    ~BoundRectStart(){}

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
    int _nHitsCut;
    int _nDivWidth;

    double _deg2rad;
  };
  
  /**
     \class larcv::BoundRectStartFactory
     \brief A concrete factory class for larcv::BoundRectStart
   */
  class BoundRectStartFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    BoundRectStartFactory() { ClusterAlgoFactory::get().add_factory("BoundRectStart",this); }
    /// dtor
    ~BoundRectStartFactory() {}
    /// creation method
    ClusterAlgoBase* create(const std::string instance_name) { return new BoundRectStart(instance_name); }
  };
  /// Global larcv::BoundRectStartFactory to register ClusterAlgoFactory
  static BoundRectStartFactory __global_BoundRectStartFactory__;
}
#endif
/** @} */ // end of doxygen group 

