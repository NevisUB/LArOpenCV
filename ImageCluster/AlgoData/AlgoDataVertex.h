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

  namespace data {

    class Refine2DVertexData;
    
    /**
       \class Vertex2D
       @brief A simple 2D vertex class with a sense of "goodness" (score, algorithm dependent)
    */
    class Vertex2D {
    public:
      Vertex2D() {clear();}
      ~Vertex2D() {}
      
      void clear() { pt.x = pt.y = -1; score = -1; }
      
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
      friend class Refine2DVertexData;
    public:
      Vertex3D(size_t id=kINVALID_SIZE) : _id(id) {clear();}
      ~Vertex3D() {}
      
      /// id retriever
      size_t id() const { return _id; }
      /// attribute clear method
      void clear() { vtx2d_v.clear(); x = y = z = kINVALID_DOUBLE; }
      
      /// Plane-wise 2D vertex point
      std::vector<larocv::data::Vertex2D> vtx2d_v;
      /// 3D vertex location
      double x, y, z;
    private:
      /// unique vertex id
      size_t _id;
    };
  }
}
#endif
/** @} */ // end of doxygen group
