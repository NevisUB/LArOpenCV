#ifndef __ALGODATA_LINEARVTXFILTERDATA_CXX__
#define __ALGODATA_LINEARVTXFILTERDATA_CXX__

#include "LinearVtxFilterData.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {

  namespace data {
    
    CircleSetting& CircleSettingArray::get_circle_setting(short plane_id) {
      if (plane_id > _circle_setting_v.size()) throw larbys("Requested plane id not valid");
      return _circle_setting_v[plane_id];
    }
  }
}

#endif
