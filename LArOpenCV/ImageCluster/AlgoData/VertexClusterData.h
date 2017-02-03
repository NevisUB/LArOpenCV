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

#include "LArOpenCV/Core/LArOCVTypes.h"
#include "Geo2D/Core/Vector.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "AlgoDataVertex.h"
#include "AlgoDataCluster.h"
#include "Refine2DVertexData.h"

namespace larocv {
  namespace data {

    // friend class forward declaration
    class ParticleClusterArray;

    /**
       \class ParticleCluster
       @brief A cluster associated with a vertex and hence represents a particle
    */
    class ParticleCluster : public ClusterCompound {
      friend class ParticleClusterArray;
    public:
      ParticleCluster(size_t id=kINVALID_SIZE) :
	ClusterCompound(),
	_cluster_id(id)
      { clear(); }
      ~ParticleCluster() {}

      /// attribute clear method
      void clear();
      /// cluster id retriever
      size_t id() const;

      //Always protect existing class members upon assignment
      ParticleCluster& operator=(const ClusterCompound& cluscomp) {
	ClusterCompound::operator=(cluscomp);
	return *this;
      }

      ParticleCluster& operator=(const ParticleCluster& pcluster) {
	ClusterCompound::operator=(pcluster);
	return *this;
      }
      
      GEO2D_Contour_t _ctor; ///< contour to define a cluster
      size_t _num_pixel;     ///< # of non-zero pixel from parent contour, contained in this contour
      double _qsum;          ///< charge sum
      
    private:
      size_t _cluster_id; ///< unique cluster id      
      
    };

    /**
       \class ParticleClusterArray
       @brief per-vertex collection container for track clusters across planes
    */
    class ParticleClusterArray {
    public:
      ParticleClusterArray() { clear(); }
      ~ParticleClusterArray() {}
      //
      // accessors
      //
      /// get id (from Vertex3D)
      size_t id() const;
      /// get # of planes
      size_t num_planes() const;
      /// get # clusters
      size_t num_clusters(size_t plane) const;
      /// get # non-zero pixels inside a contour that contains 2D vtx
      size_t num_pixels(size_t plane) const;
      /// get a fraction of non-zero pixels found in all particle cluster
      float  num_pixel_fraction(size_t plane) const;
      /// get a fraction of non-zero pixels found in a particular particle cluster
      float  num_pixel_fraction(size_t plane, size_t cluster_id) const;
      /// Vertex3D getter
      const larocv::data::Vertex3D& get_vertex() const;
      /// CircleVertex (plane-wise) getter
      const larocv::data::CircleVertex& get_circle_vertex(size_t plane) const;
      /// Plane-wise cluster list accessor
      const std::vector<larocv::data::ParticleCluster>& get_clusters(size_t plane) const;
      /// Plane-wise cluster list accessor
      std::vector<larocv::data::ParticleCluster>& get_clusters_writeable(size_t plane);
      //
      // modifiers
      //
      /// clear data attributes
      void clear();
      /// Vertex3D setter (CircleVertex omitted)
      void set_vertex(const larocv::data::Vertex3D& vtx);
      /// Vertex3D + CircleVertex cetter
      void set_vertex(const larocv::data::Vertex3D& vtx,
		      const std::vector<larocv::data::CircleVertex>& circle_vtx_v);
      /// sets # non-zero pixels per plane inside a contour that contains this vertex
      void set_num_pixel(size_t plane, size_t count);
      /// generate a new cluster
      ParticleCluster& make_cluster(size_t plane);
      /// Cluster (per plane) appender
      void push_back(size_t plane, const larocv::data::ParticleCluster& cluster);
      /// Cluster (per plane) appender
      void emplace_back(size_t plane, larocv::data::ParticleCluster&& cluster);
      
    private:
      /// 3D vertex + 2D projection 
      larocv::data::Vertex3D _vtx;
      /// An array of 2D clusters (per plane)
      std::vector<std::vector< larocv::data::ParticleCluster > > _cluster_vv;
      /// An array of # non-zero pixels per plane inside a contour that contains this vertex
      std::vector<size_t> _num_pixel_v;
      /// CircleVertex (per plane) ... optional
      std::vector<larocv::data::CircleVertex> _circle_vtx_v;
    };

    /**
       \class VertexClusterArray
       @brief Collection container for track clusters organized vertex-wise
    */
    class VertexClusterArray : public larocv::data::AlgoDataBase {
    public:
      VertexClusterArray()
      { Clear(); }
      ~VertexClusterArray() {}
      
      /// Simply a list of VertexCluster (i.e. vertex-wise list of clusters)
      std::vector<larocv::data::ParticleClusterArray> _vtx_cluster_v;
      
      /// Data attribute clear method
      void Clear() { _vtx_cluster_v.clear(); }
      
    };
  } 
}
#endif
/** @} */ // end of doxygen group
