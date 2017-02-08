#ifndef __VERTEXSEED_CXX__
#define __VERTEXSEED_CXX__

#include "VertexSeed.h"
#include "LArOpenCV/Core/larbys.h"

namespace larocv {

  namespace data {

    ///////////////////////////////////////////////////////////////////////
    void VertexSeed2D::store_seed(const geo2d::Vector<float>& seed)
    { _seed_v.push_back(seed); }

    void VertexSeed2D::move_seed(geo2d::Vector<float>&& seed)
    { _seed_v.emplace_back(std::move(seed)); }
    
    void VertexSeed2D::store_seeds(const std::vector<geo2d::Vector<float> >& seed_v) {
      if ( _seed_v.empty() )  {
	_seed_v = seed_v;
      } else {
	_seed_v.reserve(_seed_v.size() + seed_v.size());
	_seed_v.insert(std::end(_seed_v),
			std::begin(seed_v),
			std::end(seed_v));
      }
    }
    
    void VertexSeed2D::move_seeds(std::vector<geo2d::Vector<float> >&& seed_v) {
      if (_seed_v.empty()) {
	_seed_v = std::move(seed_v);
      } else {
	_seed_v.reserve(_seed_v.size() + seed_v.size());
	_seed_v.insert(std::end(_seed_v),
			std::make_move_iterator(std::begin(seed_v)),
			std::make_move_iterator(std::end(seed_v)));
      }
    }

    ///////////////////////////////////////////////////////////////////////
    

    
  }
}
#endif
