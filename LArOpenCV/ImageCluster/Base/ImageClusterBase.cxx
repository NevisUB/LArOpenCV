#ifndef __IMAGECLUSTERBASE_CXX__
#define __IMAGECLUSTERBASE_CXX__

#include "ImageClusterBase.h"

namespace larocv {

  ImageClusterBase::ImageClusterBase(const std::string name)
    : laropencv_base(name)
    ,_proc_time(0.)
    ,_proc_count(0)
    ,_image_set_id(kINVALID_IMAGE_SET_ID)
    ,_name(name)
    ,_profile(true)
    ,_dataman_ptr(nullptr)
  {
    LAROCV_DEBUG() << "Constructed" << std::endl;
  }

  ImageSetID_t ImageClusterBase::ImageSetID() const
  { return _image_set_id; }

  AlgorithmID_t ImageClusterBase::ID(const std::string& name) const
  {
    if(name.empty()) return _id;
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
  
}

#endif
