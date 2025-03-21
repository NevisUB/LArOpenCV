#ifndef __ALGODATA_VERTEXSEEDSDATA_H__
#define __ALGODATA_VERTEXSEEDSDATA_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/AlgoData/TrackClusterCompound.h"

namespace larocv {

  namespace data {
    
    /*
      \class VertexSeedsData
    */
    class VertexSeedsData : public larocv::data::AlgoDataBase {
      // friend class VertexSeeds;
      
    public:
      VertexSeedsData()
      { Clear();}
      ~VertexSeedsData(){}
      
      void Clear() {
	_vertex_seeds_v.clear();
	_vertex_seeds_v.resize(3);
      }

      const TrackClusterCompound& get_tracks(short plane_id);
      
      void plant_seeds(const VertexSeeds& vtx_seeds, short plane_id);
      void plant_seeds(VertexSeeds&& vtx_seeds, short plane_id);
      
      const VertexSeed2D& harvest_seeds(short plane_id) const;
      
    private:
      std::vector<VertexSeed2D> _vertex_seeds_v;
      //VertexSeed2DArray _vertex_seeds_v;
      
    };
  }
}
#endif
