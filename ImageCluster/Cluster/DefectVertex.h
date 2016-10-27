//by vic

#ifndef __DEFECTVERTEX_H__
#define __DEFECTVERTEX_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"

#include "Core/Geo2D.h"

namespace larocv {
 
  class DefectVertex : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    DefectVertex(const std::string name = "DefectVertex") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~DefectVertex(){}

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
    
    cv::Rect edge_aware_box(const cv::Mat& img,geo2d::Vector<float> center,int hwidth,int hheight);
    
  };
  
  class DefectVertexData : public AlgoDataBase {
    
  public:
    
    /// Default constructor
    DefectVertexData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    {  Clear(); }
    
    /// Default destructor
    ~DefectVertexData(){}

    /// Clear method override
    void Clear() {
      _circledata_v_v.clear();
      _circledata_v_v.resize(3);
    }
    
    void set_data() {}

    std::vector<std::vector<geo2d::Circle<float> > > _circledata_v_v;
    
  };
  
  /**
     \class larocv::DefectVertexFactory
     \brief A concrete factory class for larocv::DefectVertex
   */
  class DefectVertexFactory : public AlgoFactoryBase {
  public:
    /// ctor
    DefectVertexFactory() { AlgoFactory::get().add_factory("DefectVertex",this); }
    /// dtor
    ~DefectVertexFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new DefectVertex(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new DefectVertexData(instance_name,id);}
  };
  
}
#endif
/** @} */ // end of doxygen group 

