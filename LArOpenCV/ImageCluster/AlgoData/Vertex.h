#ifndef __ALGODATA_VERTEX_H__
#define __ALGODATA_VERTEX_H__

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "Geo2D/Core/Vector.h"
#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/Circle.h"
#include "Geo2D/Core/BoundingBox.h"
/*
  @brief: storage for 2D/3D verticies. Also includes "PointPCA" which is a local linear approximation
*/
namespace larocv {

  namespace data {

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

    enum class PointPCAType_t : unsigned
    { kUnknown, kTrack, kShower };
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

    enum class CircleVertexType_t : unsigned
    { kUnknown, kTrack, kShower, kBoth };
    /**
       \class CircleVertex
       @brief Vertex estimated by Refine2DVertex and its family (carries extra information about vertex)
    */
    class CircleVertex {
    public:
      CircleVertex() { Clear(); }
      ~CircleVertex() {}
      
      // Attribute variables
      geo2d::Vector<float> center;        ///< 2D vertex point
      geo2d::Vector<float> error;         ///< 2D vertex point error
      float radius;                       ///< 2D radius inspected around the vertex
      std::vector<larocv::data::PointPCA> xs_v; ///< List of charge deposit point on the circumference
      std::vector<float> dtheta_v;        ///< List of dtheta = angle between center=>cs vs. PCA @ xs
      float dtheta_xs;
      float dtheta_mxs0;
      float dtheta_mxs1;
      float dtheta_mxs2;
      float dtheta_mxs3;
      float weight;                       ///< A floating point "score", an indicator for how good this 2D vertex is
      geo2d::Vector<float> mid_pt0;
      geo2d::Vector<float> mid_pt1;
      geo2d::Vector<float> mid_pt2;

      /// Attribute clear method
      void Clear();
      /// Sum of dtheta_v
      float sum_dtheta() const;
      /// Mean of dtheta_v
      float mean_dtheta() const;
      
      geo2d::Circle<float> as_circle() const
      { return geo2d::Circle<float>(center,radius); }

    };


    enum class VertexType_t : unsigned 
    { kUnknown, kTime, kWire, kTrack, kShower, kEndOfTrack, kEdge };
    
    /**
       \class Vertex3D
       @brief A simple 3D vertex class with a colleciton of 2D projections
    */
    class Vertex3D : public AlgoDataArrayElementBase {
    public:
      Vertex3D()  { Clear(); }
      ~Vertex3D() {}
      
      /// attribute clear method
      void _Clear_()
      { vtx2d_v.clear(); cvtx2d_v.clear(); x = y = z = kINVALID_DOUBLE; num_planes=0; type=VertexType_t::kUnknown;}
      /// distance calculator
      double dist(const Vertex3D& pt) const;
      /// Plane-wise 2D vertex point
      std::vector<larocv::data::Vertex2D> vtx2d_v;
      /// Plane-wise 2D circle vertex point
      std::vector<larocv::data::CircleVertex> cvtx2d_v;
      /// 3D vertex location
      double x, y, z;
      /// # of valid planes used in this estimate
      size_t num_planes;
      /// type of vertex
      VertexType_t type;
    };

    /**
       \class Vertex3DArray
    */
    typedef AlgoDataArrayTemplate<Vertex3D> Vertex3DArray;
    
  }
}
#endif
/** @} */ // end of doxygen group
