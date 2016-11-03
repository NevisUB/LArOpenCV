//by vic

#ifndef __LINEARVTXFILTER_H__
#define __LINEARVTXFILTER_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"

#include "AlgoData/LinearVtxFilterData.h"

namespace larocv {
 
  class LinearVtxFilter : public larocv::ImageAnaBase {
    

  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    LinearVtxFilter(const std::string name = "LinearVtxFilter") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~LinearVtxFilter(){}

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

    geo2d::Line<float> calculate_pca(const std::vector<cv::Point_<float> > & ctor);
    geo2d::VectorArray<float> QPtOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle);
    
    float _r_min;
    float _r_max;
    float _r_div;
    float _r_cut;
    float _angle_cut;
    int _kink_threshold;
    int _thresh;

    std::vector<float> _radii_v;

    int _refine2d_algo_id;
    
  public:

    bool ScanRadii(const cv::Mat& img,const cv::Point_<float>& pt);

  };

  /**
     \class larocv::LinearVtxFilterFactory
     \brief A concrete factory class for larocv::LinearVtxFilter
  */
  class LinearVtxFilterFactory : public AlgoFactoryBase {
  public:
    /// ctor
    LinearVtxFilterFactory() { AlgoFactory::get().add_factory("LinearVtxFilter",this); }
    /// dtor
    ~LinearVtxFilterFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new LinearVtxFilter(instance_name); }
    /// data create method
    AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new LinearVtxFilterData(instance_name,id);}
  };
  
}
#endif
/** @} */ // end of doxygen group 

