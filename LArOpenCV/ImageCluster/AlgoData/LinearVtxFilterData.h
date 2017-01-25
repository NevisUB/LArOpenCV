#ifndef __ALGODATA_LINEARVTXFILTERDATA_H__
#define __ALGODATA_LINEARVTXFILTERDATA_H__

#include "LArOpenCV/ImageCluster/Base/AlgoDataBase.h"
#include "Geo2D/Core/Vector.h"

namespace larocv {

  namespace data {

    class CircleSetting {
      
    public:

      CircleSetting() :
	_local_r (-1),
	_angle   (-1)
      {}
      
      ~CircleSetting(){}

      //set of cross point paths
      std::vector< std::vector<geo2d::Vector<float> > > _xs_vv;
      
      size_t _idx1;
      size_t _idx2;
      
      float _local_r;
      float _angle; // in degrees
      
    };


    //per vertex circle setting array
    class CircleSettingArray {

    public:
      CircleSettingArray(){}

      ~CircleSettingArray(){}
      
      void Clear() {
	_circle_setting_v.clear();
	_circle_setting_v.resize(3);
      }
      
      CircleSetting& get_circle_setting(short plane_id);
      
    private:
      
      std::vector< CircleSetting > _circle_setting_v;
      
    };
      
    /*
      \class LinearVtxFilterData
    */
    class LinearVtxFilterData : public larocv::data::AlgoDataBase {
    public:
      LinearVtxFilterData()
	: _r_height(0)
	, _r_width(0)
      { Clear(); }
      
      ~LinearVtxFilterData(){}
      
      void Clear() {
	_circle_setting_array_v.clear();
	_radii_v.clear();
      }

      void Resize(int n) {
	_circle_setting_array_v.resize(n);
	for(auto& v : _circle_setting_array_v) v.Clear();	
      }
      std::vector<CircleSettingArray> _circle_setting_array_v;
      
      uint _r_height;
      uint _r_width;
      
      std::vector<float> _radii_v;
      
    };
    
  }
}
#endif
