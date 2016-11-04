//by vic

#ifndef __PCACANDIDATES_H__
#define __PCACANDIDATES_H__

#include "ImageAnaBase.h"
#include "AlgoFactory.h"
#include "AlgoData/PCACandidatesData.h"
#include "Core/Geo2D.h"

namespace larocv {
 
  class PCACandidates : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor: Name is used to identify a configuration parameter set via larocv::ImageClusterManager
    PCACandidates(const std::string name = "PCACandidates") :
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    virtual ~PCACandidates(){}

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

    geo2d::Line<float> calculate_pca(const GEO2D_Contour_t& ctor);

    AlgorithmID_t _defect_cluster_algo_id;
    float _pca_x_d_to_q;
    int   _nonzero_pixel_thresh;
    bool  _per_vertex;
  };

  /**
     \class larocv::PCACandidatesFactory
     \brief A concrete factory class for larocv::PCACandidates
  */
  class PCACandidatesFactory : public AlgoFactoryBase {
  public:
    /// ctor
    PCACandidatesFactory() { AlgoFactory::get().add_factory("PCACandidates",this); }
    /// dtor
    ~PCACandidatesFactory() {}
    /// create method
    ImageClusterBase* create(const std::string instance_name) { return new PCACandidates(instance_name); }
    /// data create method
    data::AlgoDataBase* create_data(const std::string instance_name, const AlgorithmID_t id)
    { return new data::PCACandidatesData(instance_name,id);}
  };
  
}
#endif
/** @} */ // end of doxygen group 

