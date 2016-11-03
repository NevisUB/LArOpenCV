#ifndef __ALGODATA_LINEARVTXFILTERDATA_CXX__
#define __ALGODATA_LINEARVTXFILTERDATA_CXX__

#include "LinearVtxFilterData.h"

namespace larocv {

  void LinearVtxFilterPlaneData::Clear()
  {
    
  }

  void LinearVtxFilterData::Clear()
  {
    _plane_data.clear();

    _vtx3d_kink_ass_v.clear();
    _vtx3d_curve_ass_v.clear();
    
    _plane_data.resize(3);
  }
}

#endif
