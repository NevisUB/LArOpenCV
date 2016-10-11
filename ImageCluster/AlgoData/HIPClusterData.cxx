#ifndef HIPCLUSTERDATA_CXX
#define HIPCLUSTERDATA_CXX

#include "HIPClusterData.h"
#include <iostream>

namespace larocv {

  void HIPClusterData::Clear()
  {
    _mip_ctors_v_v.clear();
    _hip_ctors_v_v.clear();
    _all_ctors_v_v.clear();

    _mip_ctors_v_v.resize(3);
    _hip_ctors_v_v.resize(3);
    _all_ctors_v_v.resize(3);

  }


  void HIPClusterData::set_data(const GEO2D_ContourArray_t& mips,
				const GEO2D_ContourArray_t& hips,
				const GEO2D_ContourArray_t& alls,
				short plane) {

    if (plane > 2 or plane < 0) throw std::exception();
    // std::cout << "Plane is : " << plane << "\n";
    // std::cout << mips.size() << "==" << _mip_ctors_v_v.size() << "\n";
    // std::cout << hips.size() << "==" << _hip_ctors_v_v.size() << "\n";
    // std::cout << alls.size() << "==" << _all_ctors_v_v.size() << "\n";
    _mip_ctors_v_v[plane] = mips;
    _hip_ctors_v_v[plane] = hips;
    _all_ctors_v_v[plane] = alls;

  }
}

#endif
