#ifndef __ALGODATA_DEFECTCLUSTERDATA_CXX__
#define __ALGODATA_DEFECTCLUSTERDATA_CXX__

#include "DefectClusterData.h"

namespace larocv {

  void DefectClusterPlaneData::Clear()
  {
  }

  void DefectClusterData::Clear()
  {
    _plane_data.clear();
    _plane_data.resize(3);
  }
}

#endif
