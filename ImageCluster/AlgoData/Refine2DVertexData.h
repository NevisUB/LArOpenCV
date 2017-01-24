/**
 * \file Refine2DVertexData.h
 *
 * \ingroup AlgoData
 * 
 * \brief Class def header for a class Refine2DVertexData and sub classes
 *
 * @author kazuhiro
 */

/** \addtogroup AlgoData

    @{*/


#ifndef __ALGODATA_REFINE2DVERTEXDATA_H__
#define __ALGODATA_REFINE2DVERTEXDATA_H__

#include "Core/Line.h"
#include "Core/Circle.h"
#include "Core/BoundingBox.h"
#include "AlgoData/AlgoDataVertex.h"
#include "Base/AlgoDataBase.h"
namespace larocv {

  namespace data {

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

    /*
      \class Refine2DVertexPlaneData
      @brief Plane-wise algorithm data created by Refine2DVertex, stores granular details of data
    */
    class Refine2DVertexPlaneData {
    public:
    Refine2DVertexPlaneData() { Clear(); }
      ~Refine2DVertexPlaneData() {}
      
      // Attribute variables
      bool _valid_plane;                             ///< true = atleast one vertex guess given by preceeding algorithms
      std::vector<larocv::data::CircleVertex> _init_vtx_v; ///< initial vertex guesses provided by preceeding algorithms
      std::vector< geo2d::Rect >  _scan_rect_v;      ///< an array of suare box used to scan circles for vtx search
      std::vector< larocv::data::CircleVertex > _circle_scan_v; ///< an array of CircleVertex scanned for searching true vtx 
      
      /// Attribute clear method
      void Clear();
    };
    
    /*
      \class Refine2DVertexData
      @brief Algorithm data created by Refine2DVertex, stores essential attributes for analysis
    */
    class Refine2DVertexData : public larocv::data::AlgoDataBase {
    public:
      Refine2DVertexData()
      { Clear();}
      ~Refine2DVertexData(){}
      /// attribute clear
      void Clear();

      /// access # of vertex
      size_t num_vertex() const;
      /// access plane-wise data
      const std::vector<larocv::data::Refine2DVertexPlaneData>& get_plane_data() const;
      /// access plane-wise data via plane id
      const larocv::data::Refine2DVertexPlaneData& get_plane_data(size_t plane) const;
      /// access Vertex3D array
      const std::vector<larocv::data::Vertex3D>& get_vertex() const;
      /// access Vertex3D via ID
      const larocv::data::Vertex3D& get_vertex(size_t vtx_id) const;
      /// access Vertex3D type array
      const std::vector<unsigned short>& get_type() const;
      /// access Vertex3D type array via ID
      unsigned short get_type(size_t vtx_id) const;
      /// access CircleVertex array 
      const std::vector<std::vector<larocv::data::CircleVertex> >& get_circle_vertex() const;
      /// access CircleVertex array via ID
      const std::vector<larocv::data::CircleVertex>& get_circle_vertex(size_t vtx_id) const;
      /// access CircleVertex via ID + plane
      const larocv::data::CircleVertex& get_circle_vertex(size_t vtx_id, size_t plane) const;
      /// retrieve write-able plane data
      larocv::data::Refine2DVertexPlaneData& get_plane_data_writeable(size_t plane);
      /// insert new vertex
      void push_back(const unsigned short type,
		     const larocv::data::Vertex3D& vtx,
		     const std::vector<larocv::data::CircleVertex>& circle_v);
      /// in-place move new vertex
      void emplace_back(const unsigned short type,
			larocv::data::Vertex3D&& vtx,
			std::vector<larocv::data::CircleVertex>&& circle_v);
      
    private:
      
      /// Plane-wise, detailed algorithm data. Need knowledge to use for analysis
      std::vector<larocv::data::Refine2DVertexPlaneData> _plane_data;
      /// Reconstructed 3D vertex (+ attributes that contain 2D points)
      std::vector<larocv::data::Vertex3D> _vtx3d_v;
      /// Type of reconstructed 3D vertex
      std::vector<unsigned short> _type_v;
      /// CircleVertex for corresponding Vertex3D: outer index = vertex3d, inner index = plane
      std::vector<std::vector<larocv::data::CircleVertex> > _circle_vtx_vv;

      geo2d::VectorArray<float>   _cand_vtx_v;   ///< deprecated
      std::vector< float >        _cand_score_v; ///< deprecated
      std::vector< bool  >        _cand_valid_v; ///< deprecated
      std::vector< geo2d::VectorArray<float> > _cand_xs_vv; ///< deprecated
    };
  }
}
#endif
/** @} */ // end of doxygen group
