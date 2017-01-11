#ifndef __ALGODATA_HIPCLUSTERDATA_H__
#define __ALGODATA_HIPCLUSTERDATA_H__

#include "Base/AlgoDataBase.h"
#include "Core/Line.h"

namespace larocv {

  namespace data {

    class Cluster {

    public:
      Cluster()  :
	_npx(0),
	_qsum(0),
	_iship(false)
      { clear(); }
      
      ~Cluster() { }

      void clear() {
	_npx=0;
	_qsum=0;
	_iship=false;
      }
      
      const uint npx () const { return _npx; }
      const uint qsum() const { return _qsum; }

      void set_npx  (uint a)  { _npx = a;   }
      void set_qsum (float a) { _qsum = a;  }
      void set_iship(bool b)  { _iship = b; }
      
      float qavg() const { return _qsum / (float) _npx; } 
      
      bool iship() const { return _iship ? true : false; }
      bool ismip() const { return _iship ? false : true; }
      
    private:
      
      uint  _npx;
      float _qsum;
      bool  _iship;
      
    };
    
    /*
      \class HIPClusterPlaneData
    */
    
    class HIPClusterPlaneData  {
    public:


      HIPClusterPlaneData() { Clear(); }
      virtual ~HIPClusterPlaneData(){}

      /// Clear method override
      void Clear() {
	_mip_idx_v.clear();
	_hip_idx_v.clear();
      }
      
      const uint num_mip() const { return _mip_idx_v.size(); }
      const uint num_hip() const { return _hip_idx_v.size(); }

      std::vector<Cluster>& get_clusters() { return _clusters_v; }

      void set_mip_indicies(std::vector<size_t>& mip_idx_v)  { _mip_idx_v = mip_idx_v; }
      void set_hip_indicies(std::vector<size_t>& hip_idx_v)  { _hip_idx_v = hip_idx_v; }
      void move_mip_indicies(std::vector<size_t>&& mip_idx_v)  { _mip_idx_v = std::move(mip_idx_v); }
      void move_hip_indicies(std::vector<size_t>&& hip_idx_v)  { _hip_idx_v = std::move(hip_idx_v); }
      
      const std::vector<size_t>& get_mip_indicies() const { return _mip_idx_v;  }
      const std::vector<size_t>& get_hip_indicies() const { return _hip_idx_v;  }
      
    private:

      std::vector<Cluster> _clusters_v;
      std::vector<size_t> _mip_idx_v;
      std::vector<size_t> _hip_idx_v;
      
    };
    
    /*
      \class HIPClusterData
    */
    
    class HIPClusterData : public larocv::data::AlgoDataBase {
    public:
      HIPClusterData(std::string name="NoName", AlgorithmID_t id=0)
	: AlgoDataBase(name,id)
      { Clear(); }
      ~HIPClusterData(){}
      
      void Clear() { _plane_data_v.clear(); _plane_data_v.resize(3);}
      
      std::vector<HIPClusterPlaneData> _plane_data_v;
      
    };
    
  }
}
#endif
