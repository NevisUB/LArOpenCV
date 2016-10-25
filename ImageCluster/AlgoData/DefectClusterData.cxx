#ifndef DEFECTCLUSTERDATA_CXX
#define DEFECTCLUSTERDATA_CXX

#include "DefectClusterData.h"
#include <iostream>

namespace larocv {

  void DefectClusterData::Clear()
  {
    _n_original_clusters_v.clear();
    _atomic_ctor_v_v.clear();
    _atomic_ctor_ass_v_v.clear();
    _hull_line_v_v.clear();
    _split_line_v_v.clear();
    _atomic_defect_v_v_v.clear();

    _hull_line_v_v.resize(3);
    _split_line_v_v.resize(3);
    _n_original_clusters_v.resize(3);
    _atomic_ctor_v_v.resize(3);
    _atomic_ctor_ass_v_v.resize(3);
    _atomic_defect_v_v_v.resize(3);

    _input_id = kINVALID_ID;
  }

  void DefectClusterData::set_data(unsigned n_original_clusters,
				   const GEO2D_ContourArray_t& atomic_ctor_v,
				   const std::vector<size_t>& atomic_ctor_ass_v,
				   short plane) {

    if (plane>2 or plane<0) throw std::exception();

    _n_original_clusters_v[plane] = n_original_clusters;
    _atomic_ctor_v_v      [plane] = atomic_ctor_v;
    _atomic_ctor_ass_v_v  [plane] = atomic_ctor_ass_v;
  }

}

#endif
