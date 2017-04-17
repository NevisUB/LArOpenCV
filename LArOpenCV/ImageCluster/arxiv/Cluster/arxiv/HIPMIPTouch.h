//by vic

#ifndef __HIPMIPTOUCH_H__
#define __HIPMIPTOUCH_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

#include "Geo2D/Core/Geo2D.h"

namespace larocv {
 
  class HIPMIPTouch : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    HIPMIPTouch(const std::string name = "HIPMIPTouch") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~HIPMIPTouch(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Inherited class configuration method
    void _Configure_(const Config_t &pset);
    
    void _Process_(const larocv::Cluster2DArray_t& clusters,
		   const ::cv::Mat& img,
		   larocv::ImageMeta& meta,
		   larocv::ROI& roi);

    bool _PostProcess_(const std::vector<const cv::Mat>& img_v);

  private:

    int _hip_cluster_algo_id;
    
  };

  namespace data {
    /**
       \class HIPMIPTouchData
       @brief only for vic
    */
    class HIPMIPTouchData : public AlgoDataBase {
    
    public:
    
      /// Default constructor
      HIPMIPTouchData()
      {  Clear(); }
    
      /// Default destructor
      ~HIPMIPTouchData(){}

      /// Clear method override

      AlgorithmID_t _input_id;

      void Clear() {
	_hip_mip_touch_pts_v_v.clear();
	_hip_mip_touch_pts_v_v.resize(3);

	_input_id = kINVALID_ID;
      }

      std::vector<std::vector< geo2d::Vector<float> > > _hip_mip_touch_pts_v_v;

    };
  }

  /**
     \class larocv::HIPMIPTouchFactory
     \brief A concrete factory class for larocv::HIPMIPTouch
  */
  class HIPMIPTouchFactory : public AlgoFactoryBase {
  public:
    /// ctor
    HIPMIPTouchFactory() { AlgoFactory::get().add_factory("HIPMIPTouch",this); }
    /// dtor
    ~HIPMIPTouchFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new HIPMIPTouch(instance_name); }
  };
    
}
#endif
/** @} */ // end of doxygen group 
