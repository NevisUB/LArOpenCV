#ifndef __ALGODATA_CONTOURARRAYDATA_CXX__
#define __ALGODATA_CONTOURARRAYDATA_CXX__

#include "ContourArrayData.h"
namespace larocv {
  namespace data {

    ContourArrayData::ContourArrayData() { Clear(); }

    void ContourArrayData::Clear()
    {GEO2D_ContourArray_t::clear();}
  }
}
#endif
