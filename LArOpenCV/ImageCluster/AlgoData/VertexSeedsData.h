#ifndef __ALGODATA_VERTEXSEEDSDATA_H__
#define __ALGODATA_VERTEXSEEDSDATA_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/AlgoData/DefectClusterData.h"

namespace larocv {

  namespace data {

    class VertexSeeds;
    class VertexSeedsData;

    // store per compound
    class VertexSeeds {
      friend class VertexSeedsData;
    public:
      VertexSeeds() : _n_reserved(5)
      { Reset(); }

      ~VertexSeeds(){}
      
      void Reset() {
	_cluscomp_v.clear();
	_pcaxs_v.clear();

	_cluscomp_v.reserve(_n_reserved);
	_pcaxs_v.reserve(_n_reserved*(_n_reserved-1)/2);
      }

      const std::vector<ClusterCompound>&       get_compounds() const { return _cluscomp_v; } 
      const std::vector<geo2d::Vector<float> >& get_pcaxs()     const { return _pcaxs_v; }
      
      void store_compound(const ClusterCompound& compound);
      void store_compound(ClusterCompound&& compound);
      
      void store_pcaxs(const std::vector<geo2d::Vector<float> >& pcaxs_v);
      void store_pcaxs(std::vector<geo2d::Vector<float> >&& pcaxs_v);
      
    private:
       int _n_reserved;
      
      std::vector<ClusterCompound> _cluscomp_v;
      std::vector<geo2d::Vector<float> > _pcaxs_v;
      
    };
    
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
      
      const VertexSeeds& get_seeds(short plane_id) const;
      
      void plant_seeds(const VertexSeeds& vtx_seeds, short plane_id);
      void plant_seeds(VertexSeeds&& vtx_seeds, short plane_id);

      std::vector<geo2d::Vector<float> > harvest_seed_points(short plane_id) const;
      
      const VertexSeeds& harvest_seeds(short plane_id) const;
      
    private:
      std::vector<VertexSeeds> _vertex_seeds_v;
      
    };
  }
}
#endif
