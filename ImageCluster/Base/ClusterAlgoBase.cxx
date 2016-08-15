#ifndef __CLUSTERALGOBASE_CXX__
#define __CLUSTERALGOBASE_CXX__

#include "ClusterAlgoBase.h"

namespace larocv {

  ClusterAlgoBase::ClusterAlgoBase(const std::string name)
    : ImageClusterBase(name)
  {LAROCV_DEBUG((*this)) << "Constructed" << std::endl;}

  void ClusterAlgoBase::Configure(const ::fcllite::PSet& cfg) {

    LAROCV_DEBUG((*this)) << "start" << std::endl;

    ImageClusterBase::Configure(cfg);
	
    this->set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity",(unsigned short)(this->logger().level()))));

    this->_Configure_(cfg);

    LAROCV_DEBUG((*this)) << "end" << std::endl;
  }

  Cluster2DArray_t ClusterAlgoBase::Process(const larocv::Cluster2DArray_t& clusters,
					    const ::cv::Mat& img,
					    larocv::ImageMeta& meta,
					    larocv::ROI& roi)
  {
    if(!Profile()) return this->_Process_(clusters,img,meta,roi);
    _watch.Start();
    auto result = this->_Process_(clusters,img,meta,roi);
    _proc_time += _watch.WallTime();
    ++_proc_count;
    return result;
  }

}

#endif
