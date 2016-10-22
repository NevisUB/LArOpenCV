#ifndef __RECLUSTERALGOBASE_CXX__
#define __RECLUSTERALGOBASE_CXX__

#include "ReClusterAlgoBase.h"

namespace larocv {

  ReClusterAlgoBase::ReClusterAlgoBase(const std::string name)
    : ImageClusterBase(name)
  {LAROCV_DEBUG() << "Constructed" << std::endl;}

  void ReClusterAlgoBase::Configure(const ::fcllite::PSet& cfg) {

    LAROCV_DEBUG() << "start" << std::endl;

    ImageClusterBase::Configure(cfg);
	
    this->set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity",(unsigned short)(this->logger().level()))));

    this->_Configure_(cfg);

    LAROCV_DEBUG() << "end" << std::endl;
  }

  Cluster2DArray_t ReClusterAlgoBase::Process(const larocv::Cluster2DArray_t& clusters,
					      const ::cv::Mat& img,
					      larocv::ImageMeta& meta)
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
