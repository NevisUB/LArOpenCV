#ifndef __ALGODATA_SINGLESHOWER_H__
#define __ALGODATA_SINGLESHOWER_H__

#include "Base/AlgoDataBase.h"
#include "AlgoDataVertex.h"
#include "Core/larbys.h"
namespace larocv {

  namespace data {

    class ShowerCluster {
    public:
      ShowerCluster()  { clear(); }
      ~ShowerCluster() {}
      void clear(){
	ctor.clear();
	start.x = start.y = kINVALID_FLOAT;
	dir.x = dir.y = kINVALID_FLOAT;
      }

      GEO2D_Contour_t ctor;
      geo2d::Vector<float> start;
      geo2d::Vector<float> dir;
      
    };
    
    class SingleShower {
    public:
      SingleShower(size_t id=kINVALID_SIZE) : _id(id)
      {clear();}
      ~SingleShower() {}

      void clear() {
	_cluster_v.clear();
	_vtx3d.clear();
	_ass_id =kINVALID_SIZE;
	_ass_type = -1;
      }

      const std::vector<larocv::data::ShowerCluster>& get_clusters() const
      { return _cluster_v; }

      const ShowerCluster& get_cluster(size_t plane) const
      {
	if(plane>=_cluster_v.size()) throw larbys("Invalid plane ID requested");
	return _cluster_v[plane];
      }

      const Vertex3D& get_vertex() const { return _vtx3d; }

      size_t id() const { return _id; }

      size_t ass_id() const { return _ass_id; }
      size_t ass_type() const { return _ass_type; }

      void set_association(size_t ass_id, short ass_type )
      {
	if(ass_type<0) throw larbys("Invalid ass type...");
	_ass_id   = ass_id;
	_ass_type = ass_type;
      }

      void set_vertex(const Vertex3D& vtx3d)
      { _vtx3d = vtx3d; }

      void insert(size_t plane, const ShowerCluster& cluster)
      {
	if(_cluster_v.size()<=plane) _cluster_v.resize(plane+1);
	_cluster_v[plane]=cluster;
      }

      void move(size_t plane, ShowerCluster&& cluster)
      {
	if(_cluster_v.size()<=plane) _cluster_v.resize(plane+1);
	_cluster_v[plane]=std::move(cluster);
      }

    protected:
      std::vector<larocv::data::ShowerCluster> _cluster_v;
      Vertex3D _vtx3d;
      size_t _id;
      size_t _ass_id;
      short  _ass_type;
    };

    /*
      \class SingleShowerArray
    */
    class SingleShowerArray : public larocv::data::AlgoDataBase {
    public:
      SingleShowerArray()
      { Clear(); }
      ~SingleShowerArray(){}
      
      void Clear() { _shower_v.clear(); }

      size_t num_showers() const { return _shower_v.size(); }

      const std::vector<larocv::data::SingleShower>& get_showers() const
      { return _shower_v; }

      void insert(size_t id, const SingleShower& shower)
      {
	if(_shower_v.size()<=id) _shower_v.resize(id+1);
	_shower_v[id] = shower;
      }

      void move(size_t id, SingleShower&& shower)
      {
	if(_shower_v.size()<=id) _shower_v.resize(id+1);
	_shower_v[id] = std::move(shower);
      }
      
    private:

      std::vector<larocv::data::SingleShower> _shower_v;
      
    };
    
  }
}
#endif
