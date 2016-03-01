#ifndef __MATCHALGOBASE_CXX__
#define __MATCHALGOBASE_CXX__

#include "MatchAlgoBase.h"

namespace larcv {

  MatchAlgoBase::MatchAlgoBase(const std::string name)
    : ImageClusterBase(name)
  {LARCV_DEBUG((*this)) << "Constructed" << std::endl;}

  void MatchAlgoBase::Configure(const ::fcllite::PSet& cfg) {

    LARCV_DEBUG((*this)) << "start" << std::endl;

    ImageClusterBase::Configure(cfg);
	
    this->set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity",(unsigned short)(this->logger().level()))));

    this->_Configure_(cfg);

    LARCV_DEBUG((*this)) << "end" << std::endl;
  }

  double MatchAlgoBase::Process(const larcv::Cluster2DPtrArray_t& clusters)
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
