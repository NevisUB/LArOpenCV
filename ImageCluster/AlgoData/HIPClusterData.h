#ifndef __ALGODATA_HIPCLUSTERDATA_H__
#define __ALGODATA_HIPCLUSTERDATA_H__

#include "Base/AlgoDataBase.h"
#include "Core/Line.h"
#include <set>

namespace larocv {

  namespace data {

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
