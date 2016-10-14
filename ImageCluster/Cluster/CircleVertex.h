//by vic

#ifndef __CIRCLEVERTEX_H__
#define __CIRCLEVERTEX_H__

#include "ClusterAlgoBase.h"
#include "ClusterAlgoFactory.h"

#include "Core/Geo2D.h"

#include "CircleVertexData.h"

namespace larocv {
 
  class CircleVertex : public larocv::ClusterAlgoBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    CircleVertex(const std::string name = "CircleVertex") :
      ClusterAlgoBase(name)
    {}
    
    /// Default destructor
    virtual ~CircleVertex(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    larocv::Cluster2DArray_t _Process_(const larocv::Cluster2DArray_t& clusters,
				      const ::cv::Mat& img,
				      larocv::ImageMeta& meta,
				      larocv::ROI& roi);

  private:

    unsigned _max_radius_size;
  };

  /**
     \class larocv::CircleVertexFactory
     \brief A concrete factory class for larocv::CircleVertex
   */
  class CircleVertexFactory : public ClusterAlgoFactoryBase {
  public:
    /// ctor
    CircleVertexFactory() { ClusterAlgoFactory::get().add_factory("CircleVertex",this); }
    /// dtor
    ~CircleVertexFactory() {}
    /// create method
    ClusterAlgoBase* create(const std::string instance_name) { return new CircleVertex(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new CircleVertexData(instance_name,id);}
  };
  /// Global larocv::CircleVertexFactory to register ClusterAlgoFactory
  static CircleVertexFactory __global_CircleVertexFactory__;
  
}
#endif
/** @} */ // end of doxygen group 

