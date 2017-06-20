#ifndef __ALGODATA_VERTEXESTIMATEDATA_H__
#define __ALGODATA_VERTEXESTIMATEDATA_H__

#include "Geo2D/Core/Line.h"
#include "Geo2D/Core/Circle.h"
#include "Geo2D/Core/BoundingBox.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataVertex.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"

namespace larocv {

  namespace data {

    /*
      \class VertexEstimateData
      @brief Algorithm data created by Refine2DVertex, stores essential attributes for analysis
    */
    class VertexEstimateData : public AlgoDataBase {
    public:
      VertexEstimateData()
      { Clear();}
      ~VertexEstimateData(){}
      /// attribute clear
      void Clear();

      /// access # of vertex
      size_t num_vertex() const;
      /// access Vertex3D array
      const std::vector<Vertex3D>& get_vertex() const;
      /// access Vertex3D via ID
      const Vertex3D& get_vertex(size_t vtx_id) const;
      /// access Vertex3D type array
      const std::vector<unsigned short>& get_type() const;
      /// access Vertex3D type array via ID
      unsigned short get_type(size_t vtx_id) const;
      /// access CircleVertex array 
      const std::vector<std::vector<CircleVertex> >& get_circle_vertex() const;
      /// access CircleVertex array via ID
      const std::vector<CircleVertex>& get_circle_vertex(size_t vtx_id) const;
      /// access CircleVertex via ID + plane
      const CircleVertex& get_circle_vertex(size_t vtx_id, size_t plane) const;
      /// insert new vertex
      void push_back(const unsigned short type,
		     const Vertex3D& vtx);
      /// in-place move new vertex
      void emplace_back(const unsigned short type,
			Vertex3D&& vtx);
      /// insert new vertex
      void push_back(const unsigned short type,
		     const Vertex3D& vtx,
		     const std::vector<CircleVertex>& circle_v);
      /// in-place move new vertex
      void emplace_back(const unsigned short type,
			Vertex3D&& vtx,
			std::vector<CircleVertex>&& circle_v);
      
    private:

      /// # of planes
      size_t _num_planes;
      /// Reconstructed 3D vertex (+ attributes that contain 2D points)
      std::vector<Vertex3D> _vtx3d_v;
      /// Type of reconstructed 3D vertex
      std::vector<unsigned short> _type_v;
      /// CircleVertex for corresponding Vertex3D: outer index = vertex3d, inner index = plane
      std::vector<std::vector<CircleVertex> > _circle_vtx_vv;

    };
  }
}
#endif
/** @} */ // end of doxygen group