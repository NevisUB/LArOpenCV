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
#include "AlgoDataParticle.h"


namespace larocv {
  namespace data {

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
