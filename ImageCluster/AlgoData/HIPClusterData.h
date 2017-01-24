#ifndef __ALGODATA_HIPCLUSTERDATA_H__
#define __ALGODATA_HIPCLUSTERDATA_H__

#include "Base/AlgoDataBase.h"
#include "Core/Line.h"
#include "Core/Vector.h"

namespace larocv {

  namespace data {

    class Cluster {

    public:
      Cluster()  :
	_npx(0),
	_qsum(0),
	_iship(false),
	_center_pt(kINVALID_FLOAT,kINVALID_FLOAT),
	_length(kINVALID_FLOAT),
	_width(kINVALID_FLOAT)
      { clear(); }
      
      ~Cluster() { }

      void clear() {
	_npx=0;
	_qsum=0;
	_iship=false;
      }
      
      const uint npx () const { return _npx; }
      const uint qsum() const { return _qsum; }
      const float length() const { return _length; }
      const float width() const { return _width; }
      const std::vector<cv::Point_<float> >&  px() const { return _px_v; }
      
      const std::vector<float> pixel_v()     const { return _pixel_v ;}
      const std::vector<float> mip_pixel_v() const { return _mip_pixel_v ;}  
      const std::vector<float> hip_pixel_v() const { return _hip_pixel_v ;}
      const float angle() const { return _angle; }
      
      void set_npx  (uint a)  { _npx = a;   }
      void set_qsum (float a) { _qsum = a;  }
      void set_iship(bool b)  { _iship = b; }
      void set_center_pt(geo2d::Vector<float> pt) { _center_pt = pt; }
      void set_length(float len) { _length = len; }
      void set_width(float len)  { _width  = len; }
      void set_angle(float len)  { _angle  = len; }
      void set_pixel(std::vector<float> a)     {_pixel_v = a;}
      void set_mip_pixel(std::vector<float> a) {_mip_pixel_v = a;}
      void set_hip_pixel(std::vector<float> a) {_hip_pixel_v = a;}
      void set_px(std::vector<cv::Point_<float> >& px_v) { _px_v = px_v; }
      
      float qavg() const { return _qsum / (float) _npx; } 
      
      bool iship() const { return _iship ? true : false; }
      bool ismip() const { return _iship ? false : true; }
      
    private:
      
      uint  _npx;
      float _qsum;
      bool  _iship;

      geo2d::Vector<float> _center_pt;
      float _length;
      float _width;
      float _angle;
      
      std::vector<float> _pixel_v;
      std::vector<float> _mip_pixel_v;
      std::vector<float> _hip_pixel_v;
      std::vector<cv::Point_<float> > _px_v;
      
    };
    
    /*
      \class HIPClusterPlaneData
    */
    
    class HIPClusterPlaneData  {
    public:


      HIPClusterPlaneData() :
	_long_mip_idx(kINVALID_SIZE),
	_long_hip_idx(kINVALID_SIZE)
      { Clear(); }
      
      virtual ~HIPClusterPlaneData(){}

      /// Clear method override
      void Clear() {
	_clusters_v.clear();
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

      void set_longest_mip_idx(size_t a) { _long_mip_idx = a; }
      void set_longest_hip_idx(size_t a) { _long_hip_idx = a; }
      
    private:

      std::vector<Cluster> _clusters_v;
      std::vector<size_t> _mip_idx_v;
      std::vector<size_t> _hip_idx_v;

      size_t _long_hip_idx;
      size_t _long_mip_idx;
      
    };
    
    /*
      \class HIPClusterData
    */
    
    class HIPClusterData : public larocv::data::AlgoDataBase {
    public:
      HIPClusterData()
      { Clear(); }
      ~HIPClusterData(){}
      
      void Clear() { _plane_data_v.clear(); _plane_data_v.resize(3);}
      
      std::vector<HIPClusterPlaneData> _plane_data_v;
      
    };
    
  }
}
#endif
