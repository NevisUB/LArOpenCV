//by vic

#ifndef __CIRCLEVERTEX_H__
#define __CIRCLEVERTEX_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"

#include "Core/Geo2D.h"

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
  
  class CircleVertexData : public AlgoDataBase {
    
  public:
    
    /// Default constructor
    CircleVertexData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    {  Clear(); }
    
    /// Default destructor
    ~CircleVertexData(){}

    /// Clear method override
    void Clear() {
      _circledata_v_v.clear();
      _circledata_v_v.resize(3);
    }
    
    void set_data() {}

    std::vector<std::vector<geo2d::Circle<float> > > _circledata_v_v;
    
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
