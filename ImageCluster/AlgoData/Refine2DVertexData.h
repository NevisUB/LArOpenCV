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
#include "AlgoData/Vertex.h"
#include "Base/AlgoDataBase.h"
namespace larocv {

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
    std::vector<larocv::PointPCA> xs_v; ///< List of charge deposit point on the circumference
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
    std::vector<larocv::CircleVertex> _init_vtx_v; ///< initial vertex guesses provided by preceeding algorithms
    std::vector< geo2d::Rect >  _scan_rect_v;      ///< an array of suare box used to scan circles for vtx search
    std::vector< larocv::CircleVertex > _circle_scan_v; ///< an array of CircleVertex scanned for searching true vtx 

    /// Attribute clear method
    void Clear();
  };

  /*
    \class Refine2DVertexData
    @brief Algorithm data created by Refine2DVertex, stores essential attributes for analysis
   */
  class Refine2DVertexData : public larocv::AlgoDataBase {
  public:
    Refine2DVertexData(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    { Clear();}
    ~Refine2DVertexData(){}

    void Clear();
    /// Plane-wise, detailed algorithm data. Need knowledge to use for analysis
    std::vector<larocv::Refine2DVertexPlaneData> _plane_data;
    /// Reconstructed 3D vertex (+ attributes that contain 2D points)
    std::vector<larocv::Vertex3D> _vtx3d_v;
    /// CircleVertex for corresponding Vertex3D: outer index = vertex3d, inner index = plane
    std::vector<std::vector<larocv::CircleVertex> > _circle_vtx_vv;

    geo2d::VectorArray<float>   _cand_vtx_v;   ///< deprecated
    std::vector< float >        _cand_score_v; ///< deprecated
    std::vector< bool  >        _cand_valid_v; ///< deprecated
    std::vector< geo2d::VectorArray<float> > _cand_xs_vv; ///< deprecated
  };
}
#endif
/** @} */ // end of doxygen group
