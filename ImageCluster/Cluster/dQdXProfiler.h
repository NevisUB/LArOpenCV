#ifndef __DQDXPROFILER_H__
#define __DQDXPROFILER_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"
#include "Core/VectorArray.h"

namespace larocv {
 
  class dQdXProfiler : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    dQdXProfiler(const std::string name = "dQdXProfiler") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~dQdXProfiler(){}

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
    AlgorithmID_t _pca_algo_id;
    float _pi_threshold;
    float _dx_resolution;
  };

  class dQdXProfilerData : public AlgoDataBase {
  public:
    dQdXProfilerData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~dQdXProfilerData() {}

    void Clear() {
      _pt2cluster_vv.resize(3);
      _pts_vv.resize(3);
      _dqdx_vv.resize(3);
      _pt2dist_vv.resize(3);
      _bounds_vv.resize(3);
      _o_dqdx_vvv.resize(3);
      for(size_t i=0; i<3; ++i) {
	_pt2cluster_vv[i].clear();
	_pts_vv[i].clear();
	_dqdx_vv[i].clear();
	_pt2dist_vv[i].clear();
	_bounds_vv[i].clear();
	_o_dqdx_vvv[i].clear();
      }
    }

    std::vector<std::vector<size_t> > _pt2cluster_vv;
    std::vector<geo2d::VectorArray<int> > _pts_vv;
    std::vector<std::vector<float> > _dqdx_vv;
    std::vector<std::vector<float> > _pt2dist_vv;
    std::vector<std::vector<std::pair<float,float> > > _bounds_vv;
    std::vector<std::vector<std::vector<float> > > _o_dqdx_vvv;
    
  };

  /**
     \class larocv::dQdXProfilerFactory
     \brief A concrete factory class for larocv::dQdXProfiler
   */
  class dQdXProfilerFactory : public AlgoFactoryBase {
  public:
    /// ctor
    dQdXProfilerFactory() { AlgoFactory::get().add_factory("dQdXProfiler",this); }
    /// dtor
    ~dQdXProfilerFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new dQdXProfiler(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new dQdXProfilerData(instance_name,id);}
  };
  
}
#endif
/** @} */ // end of doxygen group 

