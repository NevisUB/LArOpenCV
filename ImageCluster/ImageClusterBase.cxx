#ifndef __IMAGECLUSTERBASE_CXX__
#define __IMAGECLUSTERBASE_CXX__

#include "ImageClusterBase.h"

namespace larcv {

  ImageClusterBase::ImageClusterBase(const std::string name)
    : laropencv_base(name)
    , _name(name)
    , _proc_time(0.)
    , _proc_count(0)
    , _profile(true)
    , _alg_m(nullptr)
    , _var_v(nullptr)
  {LARCV_DEBUG((*this)) << "Constructed" << std::endl;}

  void ImageClusterBase::Configure(const ::fcllite::PSet& cfg) {

    LARCV_DEBUG((*this)) << "start" << std::endl;
    this->set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity",(unsigned short)(this->logger().level()))));
    _profile = cfg.get<bool>("Profile",_profile);

    this->_Configure_(cfg);

    LARCV_DEBUG((*this)) << "end" << std::endl;
  }

  ContourArray_t ImageClusterBase::Process(const larcv::ContourArray_t& clusters,
					   const ::cv::Mat& img,
					   larcv::ImageMeta& meta)
  {
    if(!_profile) return this->_Process_(clusters,img,meta);
    _watch.Start();
    auto result = this->_Process_(clusters,img,meta);
    _proc_time += _watch.WallTime();
    ++_proc_count;
    return result;
  }

  larcv::AlgorithmID_t ImageClusterBase::AlgoID(const std::string algo_name)
  {
    if(!_alg_m)

      throw larbys("FMWK not configured to correlated algorithm name and iD!");
    
    auto iter = _alg_m->find(algo_name);
    if(iter == _alg_m->end()) {
      LARCV_ERROR((*this)) << "Algorithm name " << algo_name << " not found!" << std::endl;
      throw larbys("Cannot fetch AlgorithmID_t!");
    }
    return (*iter).second;
  }
}

#endif
