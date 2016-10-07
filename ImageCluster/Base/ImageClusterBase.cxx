#ifndef __IMAGECLUSTERBASE_CXX__
#define __IMAGECLUSTERBASE_CXX__

#include "ImageClusterBase.h"

namespace larocv {

  ImageClusterBase::ImageClusterBase(const std::string name)
    : laropencv_base(name)
    ,_proc_time(0.)
    ,_proc_count(0)
    ,_name(name)
    ,_profile(true)
    ,_dataman_ptr(nullptr)
{
    LAROCV_DEBUG((*this)) << "Constructed" << std::endl;
  }

  void ImageClusterBase::Configure(const ::fcllite::PSet& cfg) {
    LAROCV_DEBUG((*this)) << "start" << std::endl;
    this->set_verbosity((msg::Level_t)(cfg.get<unsigned short>(
							       "Verbosity", (unsigned short)(this->logger().level()))));
    _profile = cfg.get<bool>("Profile", _profile);

    LAROCV_DEBUG((*this)) << "end" << std::endl;
  }
}

#endif
