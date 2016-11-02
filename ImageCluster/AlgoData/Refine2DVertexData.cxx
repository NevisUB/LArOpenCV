#ifndef __ALGODATA_REFINE2DVERTEXDATA_CXX__
#define __ALGODATA_REFINE2DVERTEXDATA_CXX__

#include "Refine2DVertexData.h"

namespace larocv {

  void CircleVertex::Clear()
  { center.x = center.y = -1;
    error.x  = error.y  = -1;
    radius   = -1;
    xs_v.clear();
    dtheta_v.clear();
  }

  float CircleVertex::sum_dtheta() const
  { float res=0; for(auto const& d : dtheta_v) res += d; return res; }

  void Refine2DVertexPlaneData::Clear()
  {
    _valid_plane = false;
    _init_vtx_v.clear();
    _scan_rect_v.clear();
    _circle_scan_v.clear();
  }

  void Refine2DVertexData::Clear() {

    _plane_data.resize(3);
    for(auto& d : _plane_data) d.Clear();
    
    _vtx3d_v.clear();
    _circle_vtx_vv.clear();
    
    _cand_valid_v.clear();
    _cand_score_v.clear();
    _cand_vtx_v.clear();
    
    _cand_valid_v.resize(3,false);
    _cand_score_v.resize(3,-1);
    _cand_vtx_v.resize(3);
    
    _cand_xs_vv.resize(3);
    for(auto& d : _cand_xs_vv)      d.clear();
    
  }
}

#endif
