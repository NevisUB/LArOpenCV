#ifndef LARLITE_MCGETTER_CXX
#define LARLITE_MCGETTER_CXX

#include "MCGetter.h"

namespace larlite {

  bool MCGetter::initialize() {

    return true;
  }
  
  bool MCGetter::analyze(storage_manager* storage) {

    showers_ = storage->get_data<event_mcshower>("mcreco");
    
    return true;
  }

  bool MCGetter::finalize() {

    return true;
  }

}
#endif
