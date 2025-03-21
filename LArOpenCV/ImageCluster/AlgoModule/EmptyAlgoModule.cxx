#ifndef __EMPTYALGOMODULE_CXX__
#define __EMPTYALGOMODULE_CXX__

#include "EmptyAlgoModule.h"

namespace larocv {

  /// Global larocv::EmptyAlgoModuleFactory to register AlgoFactory
  static EmptyAlgoModuleFactory __global_EmptyAlgoModuleFactory__;
  
  void EmptyAlgoModule::_Configure_(const Config_t &pset)
  {
  }

  bool EmptyAlgoModule::_PostProcess_() const
  { return true; }

  void EmptyAlgoModule::_Process_() {

    auto img_v  = ImageArray();
    auto meta_v = MetaArray();

 
  }
}
#endif
