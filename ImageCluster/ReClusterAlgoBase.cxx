#ifndef __RECLUSTERALGOBASE_CXX__
#define __RECLUSTERALGOBASE_CXX__

#include "ReClusterAlgoBase.h"

namespace larcv {

  ReClusterAlgoBase::ReClusterAlgoBase(const std::string name)
    : ImageClusterBase(name)
  {LARCV_DEBUG((*this)) << "Constructed" << std::endl;}

  void ReClusterAlgoBase::Configure(const ::fcllite::PSet& cfg) {

    LARCV_DEBUG((*this)) << "start" << std::endl;

    ImageClusterBase::Configure(cfg);
	
    this->set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity",(unsigned short)(this->logger().level()))));

    this->_Configure_(cfg);

    LARCV_DEBUG((*this)) << "end" << std::endl;
  }

  Cluster2DArray_t ReClusterAlgoBase::Process(const larcv::Cluster2DArray_t& clusters,
					      const ::cv::Mat& img,
					      larcv::ImageMeta& meta)
  {
    if(!Profile()) return this->_Process_(clusters,img,meta);
    _watch.Start();
    auto result = this->_Process_(clusters,img,meta);
    _proc_time += _watch.WallTime();
    ++_proc_count;
    return result;
  }

}

#endif
