#ifndef DEFECTCLUSTERDATA_CXX
#define DEFECTCLUSTERDATA_CXX

#include "DefectClusterData.h"
#include <iostream>

namespace larocv {

  void DefectClusterData::Clear()
  {
    _atomic_ctor_v_v.clear();
    _atomic_ctor_ass_v_v.clear();

    _atomic_ctor_v_v.resize(3);
    _atomic_ctor_ass_v_v.resize(3);
  }

  void DefectClusterData::set_data(const GEO2D_ContourArray_t& atomic_ctor_v,
				   const std::vector<size_t>& atomic_ctor_ass_v,
				   short plane) {

    if (plane>2 or plane<0) throw std::exception();

    _atomic_ctor_v_v    [plane] = atomic_ctor_v;
    _atomic_ctor_ass_v_v[plane] = atomic_ctor_ass_v;
    
  }

}

#endif
