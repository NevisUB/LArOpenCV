#ifndef __ALGODATA_LINEARTRACKCLUSTER_H__
#define __ALGODATA_LINEARTRACKCLUSTER_H__

#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "LArOpenCV/ImageCluster/AlgoData/AlgoDataVertex.h"
namespace larocv {

  namespace data {

    class LinearTrackArray;

    class LinearTrack2D {
    public:
      LinearTrack2D()  {clear();}
      ~LinearTrack2D() {}
      /// attribute clear method
      void clear()
      { ctor.clear(); edge1.x = edge1.y = edge2.x = edge2.y = kINVALID_FLOAT; }

      GEO2D_Contour_t ctor; ///< contour to define a cluster
      ::geo2d::Vector<float> edge1; ///< edge 1 of the track
      ::geo2d::Vector<float> edge2; ///< edge 2 of the track
    };

    class LinearTrack {
      friend class LinearTrackArray;
    public:
      LinearTrack(size_t id=kINVALID_SIZE) : _id(id)
      { clear(); }
      ~LinearTrack() {}

      void clear()
      {
	_strack_v.clear();
	edge1.clear();
	edge2.clear();
	_start_edge = 0;
      }

      Vertex3D edge1; ///< edge 1 (start or end)
      Vertex3D edge2; ///< edge 2 (start or end)

      const LinearTrack2D& get_cluster(size_t plane) const
      {
	if(plane >= _strack_v.size()) throw larbys("Invalid plane id requested...");
	return _strack_v[plane];
      }

      const std::vector<LinearTrack2D>& get_clusters() const 
      {
	return _strack_v;
      }

      void set_start_edge(unsigned short edge)
      {
	if(edge>2) throw larbys("Invalid edge number!");
	_start_edge = edge;
      }

      const Vertex3D& start() const
      {
	if(!_start_edge) throw larbys("Start edge unknown!");
	return (_start_edge == 1 ? edge1 : edge2);
      }

      const Vertex3D& end() const
      {
	if(!_start_edge) throw larbys("End edge unknown!");
	return (_start_edge == 1 ? edge2 : edge1);
      }

      void insert(const std::vector<larocv::data::LinearTrack2D>& strack_v)
      { _strack_v = strack_v; }

      void move(std::vector<larocv::data::LinearTrack2D>&& strack_v)
      { _strack_v = std::move(strack_v); }

      void insert(size_t plane, const LinearTrack2D& strack)
      {
	_strack_v.resize(plane+1);
	_strack_v[plane] = strack;
      }

      void move(size_t plane, LinearTrack2D&& strack)
      {
	_strack_v.resize(plane+1);
	_strack_v[plane] = std::move(strack);
      }

      const size_t id() const { return _id; }
      
    private:
      unsigned short _start_edge;
      size_t _id;
      std::vector<larocv::data::LinearTrack2D> _strack_v; ///< per-plane contour
    };

    /*
      \class LinearTrackArray
    */
    class LinearTrackArray : public larocv::data::AlgoDataBase {
    public:
      LinearTrackArray()
      { Clear(); }
      ~LinearTrackArray(){}
      
      void Clear() { _strack_v.clear(); }

      const std::vector<larocv::data::LinearTrack> get_clusters() const
      { return _strack_v; }

      const LinearTrack get_cluster(size_t id) const
      {
	if(id>=_strack_v.size()) throw larbys("Invalid cluster id requested!");
	return _strack_v[id];
      }

      void push_back(const LinearTrack& strack)
      {
	_strack_v.push_back(strack);
	_strack_v.back()._id = _strack_v.size()-1;
      }

      void emplace_back(LinearTrack&& strack) 
      {
	strack._id = _strack_v.size();
	_strack_v.emplace_back(std::move(strack));
      }
      
    protected:
      std::vector<larocv::data::LinearTrack> _strack_v;
      
    };
    
  }
}
#endif
