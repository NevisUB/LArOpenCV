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
#include "Core/Line.h"
#include "Core/Circle.h"
#include "Core/BoundingBox.h"

namespace larocv {

  namespace data {

    class Refine2DVertexData;
    class SingleTrackArray;
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
      friend class SingleTrackArray;
    public:
      Vertex3D(size_t id=kINVALID_SIZE) : _id(id) {clear();}
      ~Vertex3D() {}
      
      /// id retriever
      size_t id() const { return _id; }
      /// attribute clear method
      void clear() { vtx2d_v.clear(); x = y = z = kINVALID_DOUBLE; num_planes=0; }
      
      /// Plane-wise 2D vertex point
      std::vector<larocv::data::Vertex2D> vtx2d_v;
      /// 3D vertex location
      double x, y, z;
      /// # of valid planes used in this estimate
      size_t num_planes;
    private:
      /// unique vertex id
      size_t _id;
    };

    /*
      \class PointPCA
      @brief Represent a line (actually not PCA...) approximated using PCA at a particular point
    */
    class PointPCA {
    public:
      PointPCA(){}      
      PointPCA(const geo2d::Vector<float>& in_pt,
	       const geo2d::Line<float>& in_line)
	: pt(in_pt), line(in_line)
      {}
      geo2d::Vector<float> pt;   ///< point around which PCA approximation is made
      geo2d::Line<float>   line; ///< long PCA axis of the neighboring pixels 
    };
    
    /**
       \class CircleVertex
       @brief Vertex estimated by Refine2DVertex and its family (carries extra information about vertex)
    */
    class CircleVertex {
    public:
      CircleVertex() {Clear();}
      ~CircleVertex() {}
      
      // Attribute variables
      geo2d::Vector<float> center;        ///< 2D vertex point
      geo2d::Vector<float> error;         ///< 2D vertex point error
      float radius;                       ///< 2D radius inspected around the vertex
      std::vector<larocv::data::PointPCA> xs_v; ///< List of charge deposit point on the circumference
      std::vector<float> dtheta_v;        ///< List of dtheta = angle between center=>cs vs. PCA @ xs
      
      /// Attribute clear method
      void Clear();
      /// Sum of dtheta_v
      float sum_dtheta() const;
    };


    
  }
}
#endif
/** @} */ // end of doxygen group
