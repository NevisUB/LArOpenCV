#ifndef __IMAGECLUSTERBASE_CXX__
#define __IMAGECLUSTERBASE_CXX__

#include "ImageClusterBase.h"

namespace larocv {

  ImageClusterBase::ImageClusterBase(const std::string name)
    : laropencv_base(name)
    ,_proc_time(0.)
    ,_proc_count(0)
    ,_image_set_id(ImageSetID_t::kImageSetUnknown)
    ,_name(name)
    ,_profile(true)
    ,_dataman_ptr(nullptr)
  {
    LAROCV_DEBUG() << "Constructed" << std::endl;
  }

  ImageSetID_t ImageClusterBase::DefaultImageSetID() const
  { return _image_set_id; }

  AlgorithmID_t ImageClusterBase::ID(const std::string& name) const
  {
    //if(name.empty()) return _id;
    if(name.empty()) return kINVALID_ALGO_ID;
    if(name == _name) return _id;
    if(!_dataman_ptr) {
      LAROCV_CRITICAL() << "No AlgoDataManager instance (cannot search name=>id map)" << std::endl;
      throw larbys();
    }
    return _dataman_ptr->ID(name);
  }
  
  void ImageClusterBase::Configure(const Config_t& cfg) {
    LAROCV_DEBUG() << "start" << std::endl;
    this->set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
    _profile = cfg.get<bool>("Profile", _profile);
    _image_set_id = (ImageSetID_t)(cfg.get<size_t>("ImageSetID",0));
    LAROCV_DEBUG() << "end" << std::endl;
  }

  void ImageClusterBase::Register(data::AlgoDataBase* data) 
  { _dataman_ptr->Register(_id,data); }
  
  void ImageClusterBase::SetData(const std::vector<std::vector<cv::Mat> >& image_vv,
				 const std::vector<std::vector<larocv::ImageMeta> >& meta_vv,
				 const std::vector<std::vector<larocv::ROI> >& roi_vv)
  {
    _image_vv = image_vv;
    _meta_vv  = meta_vv;
    _roi_vv   = roi_vv;
  }

  std::vector<cv::Mat> ImageClusterBase::ImageArray(ImageSetID_t image_id) const
  {
    if(image_id == ImageSetID_t::kImageSetUnknown) image_id = DefaultImageSetID();
    if(image_id == ImageSetID_t::kImageSetUnknown) {
      LAROCV_CRITICAL() << "kImageSetUnknwon is not a valid ImageSetID_t value" << std::endl;
      throw larbys();
    }
    if((int)(_roi_vv.size()) <= (int)image_id) {
      LAROCV_CRITICAL() << "ImageSetID_t " << (int)image_id << " not available!" << std::endl;
      throw larbys();
    }
    return _image_vv[(int)image_id];
  }

  const std::vector<larocv::ImageMeta>& ImageClusterBase::MetaArray(ImageSetID_t image_id) const
  {
    if(image_id == ImageSetID_t::kImageSetUnknown) image_id = DefaultImageSetID();
    if(image_id == ImageSetID_t::kImageSetUnknown) {
      LAROCV_CRITICAL() << "kImageSetUnknwon is not a valid ImageSetID_t value" << std::endl;
      throw larbys();
    }
    if((int)(_roi_vv.size()) <= (int)image_id) {
      LAROCV_CRITICAL() << "ImageSetID_t " << (int)image_id << " not available!" << std::endl;
      throw larbys();
    }
    return _meta_vv[(int)image_id];
  }

  const std::vector<larocv::ROI>& ImageClusterBase::ROIArray(ImageSetID_t image_id) const
  {
    if(image_id == ImageSetID_t::kImageSetUnknown) image_id = DefaultImageSetID();
    if(image_id == ImageSetID_t::kImageSetUnknown) {
      LAROCV_CRITICAL() << "kImageSetUnknwon is not a valid ImageSetID_t value" << std::endl;
      throw larbys();
    }
    if((int)(_roi_vv.size()) <= (int)image_id) {
      LAROCV_CRITICAL() << "ImageSetID_t " << (int)image_id << " not available!" << std::endl;
      throw larbys();
    }
    return _roi_vv[(int)image_id];
  }
  
}

#endif
