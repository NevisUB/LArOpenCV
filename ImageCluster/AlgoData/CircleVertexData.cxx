#ifndef CIRCLEVERTEXDATA_CXX
#define CIRCLEVERTEXDATA_CXX

#include "CircleVertexData.h"
#include <iostream>

namespace larocv {

  void CircleVertexData::Clear()
  {
    _ctor_lines_v_v.clear();
    _ipoints_v_v.clear();

    _ctor_lines_v_v.resize(3);
    _ipoints_v_v.resize(3);
  }


  void CircleVertexData::set_data(const std::vector<geo2d::Line<float> >& ctor_lines_v,
				 const std::vector<geo2d::Vector<float> >& ipoints_v,
				 short plane)
  {
    
    if (plane > 2 or plane < 0) /*fuck*/ throw std::exception();
    
    _ctor_lines_v_v[plane] = ctor_lines_v;
    _ipoints_v_v[plane]    = ipoints_v;

  }


}

#endif
