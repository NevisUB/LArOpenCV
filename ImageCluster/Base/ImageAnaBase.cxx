#ifndef __IMAGEANABASE_CXX__
#define __IMAGEANABASE_CXX__

#include "ImageAnaBase.h"

namespace larocv {

  ImageAnaBase::ImageAnaBase(const std::string name)
    : ImageClusterBase(name)
  {LAROCV_DEBUG() << "Constructed" << std::endl;}
  
  void ImageAnaBase::Configure(const ::fcllite::PSet& cfg) {

    LAROCV_DEBUG() << "start" << std::endl;

    ImageClusterBase::Configure(cfg);
	
    this->set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity",(unsigned short)(this->logger().level()))));

    this->_Configure_(cfg);

    LAROCV_DEBUG() << "end" << std::endl;
  }

  void ImageAnaBase::Process(const larocv::Cluster2DArray_t& clusters,
			     const ::cv::Mat& img,
			     larocv::ImageMeta& meta,
			     larocv::ROI& roi)
  {
    if(!Profile()) {
      this->_Process_(clusters,img,meta,roi);
      return;
    }
    _watch.Start();
    this->_Process_(clusters,img,meta,roi);
    _proc_time += _watch.WallTime();
    ++_proc_count;
    return;
  }

  bool ImageAnaBase::PostProcess(const std::vector<const cv::Mat>& img_v)
  {
    if(!Profile()) {
      return this->_PostProcess_(img_v);
    }
    bool state=true;
    _watch.Start();
    state = this->_PostProcess_(img_v);
    _proc_time += _watch.WallTime();
    ++_proc_count;
    return state;
  }

}

#endif
