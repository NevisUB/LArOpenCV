//vic
#ifndef __BOUNDRECTSTART_H__
#define __BOUNDRECTSTART_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

namespace larocv {

  
  class BoundRectStart : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor
    BoundRectStart(const std::string name="BoundRectStart") :
      ClusterAlgoBase(name)
      , _deg2rad ( 3.14159 / 180.0 )
      , _use_roi_vertex(false)
    {}
    
    /// Default destructor
    ~BoundRectStart(){}

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

    int _nDivWidth;

    bool _cutbadreco;
    
    double _deg2rad;

    double _use_roi_vertex;
  };
  
  /**
     \class larocv::BoundRectStartFactory
     \brief A concrete factory class for larocv::BoundRectStart
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
  /// Global larocv::BoundRectStartFactory to register ClusterAlgoFactory
  static BoundRectStartFactory __global_BoundRectStartFactory__;
}
#endif
/** @} */ // end of doxygen group 

