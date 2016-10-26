//by vic

#ifndef __CIRCLEVERTEX_H__
#define __CIRCLEVERTEX_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"

#include "Core/Geo2D.h"

#include "CircleVertexData.h"

namespace larocv {
 
  class CircleVertex : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    CircleVertex(const std::string name = "CircleVertex") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~CircleVertex(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const ::fcllite::PSet &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:

    unsigned _max_radius_size;
  };

  /**
     \class larocv::CircleVertexFactory
     \brief A concrete factory class for larocv::CircleVertex
   */
  class CircleVertexFactory : public AlgoFactoryBase {
  public:
    /// ctor
    CircleVertexFactory() { AlgoFactory::get().add_factory("CircleVertex",this); }
    /// dtor
    ~CircleVertexFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new CircleVertex(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new CircleVertexData(instance_name,id);}
  };
  
}
#endif
/** @} */ // end of doxygen group 

