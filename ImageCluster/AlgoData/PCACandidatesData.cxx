#ifndef PCACANDIDATESDATA_CXX
#define PCACANDIDATESDATA_CXX

#include "PCACandidatesData.h"

namespace larocv {

  void PCACandidatesData::Clear()
  {
    _ctor_lines_v_v_v.clear();
    _ipoints_v_v_v.clear();
    _atomic_id_v_v_v.clear();

    _ctor_lines_v_v_v.resize(3);
    _ipoints_v_v_v.resize(3);
    _atomic_id_v_v_v.resize(3);

    _input_id = kINVALID_ID;
  }


  void PCACandidatesData::set_data(const std::vector<geo2d::Line<float> >& ctor_lines_v,
				   const std::vector<geo2d::Vector<float> >& ipoints_v,
				   const std::vector<size_t>& atomic_id_v,
				   short plane)
    
  {
    
    if (plane > 2 or plane < 0) /*fuck*/ throw std::exception();
    
    _ctor_lines_v_v_v[plane].emplace_back( ctor_lines_v );
    _ipoints_v_v_v   [plane].emplace_back( ipoints_v    );
    _atomic_id_v_v_v [plane].emplace_back( atomic_id_v  );
  }


}

#endif
