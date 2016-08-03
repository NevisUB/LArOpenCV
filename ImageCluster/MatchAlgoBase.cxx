#ifndef __MATCHALGOBASE_CXX__
#define __MATCHALGOBASE_CXX__

#include "MatchAlgoBase.h"

namespace larocv {

  MatchAlgoBase::MatchAlgoBase(const std::string name)
    : ImageClusterBase(name)
  {LAROCV_DEBUG((*this)) << "Constructed" << std::endl;}

  void MatchAlgoBase::Configure(const ::fcllite::PSet& cfg) {

    LAROCV_DEBUG((*this)) << "start" << std::endl;

    ImageClusterBase::Configure(cfg);
	
    this->set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity",(unsigned short)(this->logger().level()))));

    this->_Configure_(cfg);

    LAROCV_DEBUG((*this)) << "end" << std::endl;
  }

  double MatchAlgoBase::Process(const larocv::Cluster2DPtrArray_t& clusters)
  {
    if(!Profile()) return this->_Process_(clusters);
    _watch.Start();
    auto result = this->_Process_(clusters);
    _proc_time += _watch.WallTime();
    ++_proc_count;
    return result;
  }

}

#endif
