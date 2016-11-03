/**
 * \file VertexClusterData.h
 *
 * \ingroup AlgoData
 * 
 * \brief Class def header for a class VertexClusterArray
 *
 * @author kazuhiro
 */

/** \addtogroup AlgoData

    @{*/

#ifndef __ALGODATA_VERTEXCLUSTERDATA_H__
#define __ALGODATA_VERTEXCLUSTERDATA_H__

#include "Core/LArOCVTypes.h"
#include "Core/Vector.h"
#include "Base/AlgoDataBase.h"
#include "AlgoDataVertex.h"
#include "Refine2DVertexData.h"

namespace larocv {

  class VertexCluster {
  public:
    VertexCluster() { clear(); }
    ~VertexCluster() {}
    //
    // accessors
    //
    /// get # of planes
    size_t num_planes() const;
    /// Vertex3D getter
    const larocv::Vertex3D& get_vertex() const;
    /// CircleVertex (plane-wise) getter
    const larocv::CircleVertex& get_circle_vertex(size_t plane) const;
    /// Plane-wise cluster list accessor
    const std::vector< std::vector<geo2d::Vector<float> > >& get_clusters(size_t plane) const;
    //
    // modifiers
    //
    /// clear data attributes
    void clear();
    /// set # of planes
    void num_planes(size_t n);
    /// Vertex3D setter (CircleVertex omitted)
    void set_vertex(const larocv::Vertex3D& vtx);
    /// Vertex3D + CircleVertex cetter
    void set_vertex(const larocv::Vertex3D& vtx,
		    const std::vector<larocv::CircleVertex>& circle_vtx_v);
    /// Cluster (per plane) appender
    void append(size_t plane, const std::vector<geo2d::Vector<float> >& cluster);
    /// Cluster (per plane) appender
    void emplace(size_t plane, std::vector<geo2d::Vector<float> >&& cluster);
    
  private:
    /// 3D vertex + 2D projection 
    larocv::Vertex3D _vtx;
    ///< an array of 2D clusters (per plane)
    std::vector<std::vector< std::vector<geo2d::Vector<float> > > > _cluster_vv;
    /// CircleVertex (per plane) ... optional
    std::vector<larocv::CircleVertex> _circle_vtx_v;
  };

  /**
     \class VertexClusterArray
     @brief Collection container for clusters organized vertex-wise
  */
  class VertexClusterArray : public larocv::AlgoDataBase {
  public:
    VertexClusterArray(std::string name="NoName", AlgorithmID_t id=0)
      : AlgoDataBase(name,id)
    {Clear();}
    ~VertexClusterArray() {}

    /// Simply a list of VertexCluster (i.e. vertex-wise list of clusters)
    std::vector<larocv::VertexCluster> _vtx_cluster_v;

    /// Data attribute clear method
    void Clear() { _vtx_cluster_v.clear(); }

  };

}
#endif
/** @} */ // end of doxygen group
