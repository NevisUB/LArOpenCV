#ifndef __EMPTYIMAGEANA_H__
#define __EMPTYIMAGEANA_H__

#include "LArOpenCV/ImageCluster/Base/ImageAnaBase.h"
#include "LArOpenCV/ImageCluster/Base/AlgoFactory.h"

namespace larocv {
    
  class EmptyImageAna : public larocv::ImageAnaBase {
    
  public:
    
    /// Default constructor
    EmptyImageAna(const std::string name="EmptyImageAna") : 
      ImageAnaBase(name)
    {}
    
    /// Default destructor
    ~EmptyImageAna(){}

    /// Finalize after process
    void Finalize(TFile*) {}

  protected:

    /// Configuration method
    void _Configure_(const Config_t &pset);

    /// Process method
    void _Process_(const Cluster2DArray_t& clusters,
		   ::cv::Mat& img,
		   ImageMeta& meta,
		   ROI& roi);

    bool _PostProcess_(std::vector<cv::Mat>& img_v)
    { return true; }
    
  private:

  };

  class EmptyImageAnaFactory : public AlgoFactoryBase {
  public:
    EmptyImageAnaFactory() { AlgoFactory::get().add_factory("EmptyImageAna",this); }
    ~EmptyImageAnaFactory() {}
    ImageAnaBase* create(const std::string instance_name) { return new EmptyImageAna(instance_name); }
  };

}
#endif
/** @} */ // end of doxygen group 

