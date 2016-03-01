#ifndef LARLITE_MCGETTER_H
#define LARLITE_MCGETTER_H

#include "Analysis/ana_base.h"
#include "DataFormat/mcshower.h"
#include "LArUtil/GeometryUtilities.h"

namespace larlite {
  class MCGetter : public ana_base {
  
  public:

    MCGetter() : showers_(nullptr) { _name="MCGetter"; _fout=0;}
    virtual ~MCGetter(){}
    virtual bool initialize();
    virtual bool analyze(storage_manager* storage);
    virtual bool finalize();

    event_mcshower* showers_;
    
  };
}
#endif
