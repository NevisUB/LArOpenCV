/**
 * \file Vertex.h
 *
 * \ingroup AlgoData
 * 
 * \brief Class def header for a class Vertex2D and Vertex3D
 *
 * @author kazuhiro
 */

/** \addtogroup AlgoData

    @{*/

#ifndef __ALGODATA_VERTEX_H__
#define __ALGODATA_VERTEX_H__

#include "Core/LArOCVTypes.h"
#include "Core/Vector.h"

namespace larocv {

  /**
     \class Vertex2D
     @brief A simple 2D vertex class with a sense of "goodness" (score, algorithm dependent)
  */
  class Vertex2D {
  public:
    Vertex2D() {Clear();}
    ~Vertex2D() {}

    void Clear() { pt.x = pt.y = -1; score = -1; }
    
    /// 2D vertex point
    geo2d::Vector<float> pt;
    /// Associated score, definition is algorithm dependent
    float score;
  };

  /**
     \class Vertex3D
     @brief A simple 3D vertex class with a colleciton of 2D projections
  */
  class Vertex3D {
  public:
    Vertex3D() {Clear();}
    ~Vertex3D() {}

    void Clear() { vtx2d_v.clear(); x = y = z = kINVALID_DOUBLE; }

    /// Plane-wise 2D vertex point
    std::vector<larocv::Vertex2D> vtx2d_v;
    /// 3D vertex location
    double x, y, z;
  };

}
#endif
