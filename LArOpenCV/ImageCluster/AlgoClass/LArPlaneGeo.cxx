#ifndef __LARPLANEGEO_CXX__
#define __LARPLANEGEO_CXX__

#include "LArOpenCV/ImageCluster/Base/ImageClusterFMWKInterface.h"
#include "LArUtil/LArUtilException.h"
#include "LArPlaneGeo.h"

namespace larocv {

  LArPlaneGeo::LArPlaneGeo()
  {
    _xplane_tick_resolution = 3;
    _trigger_tick = 3200;
    _num_planes = 3;

    _tick_offset_v.clear();
    _tick_offset_v.resize(_num_planes);
    _tick_offset_v[0] = 0.;
    _tick_offset_v[1] = 4.54;
    _tick_offset_v[2] = 7.78;
    //_tick_offset_v[1] = 0.;
    //_tick_offset_v[2] = 0.;

    Reset();
  }

  void LArPlaneGeo::Reset()
  {
    _wire_comp_factor_v.clear();
    _time_comp_factor_v.clear();
    _origin_v.clear();
    _wire_comp_factor_v.resize(_num_planes,1);
    _time_comp_factor_v.resize(_num_planes,1);
    _num_cols_v.resize(_num_planes,0);
    _num_rows_v.resize(_num_planes,0);
    _origin_v.resize(_num_planes);
  }

  void LArPlaneGeo::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity",2)));
    _xplane_tick_resolution=pset.get<float>("XPlaneTickResolution",3);
    _trigger_tick=pset.get<float>("TriggerTick",3200);
    
    Reset();
  }

  
  void LArPlaneGeo::ResetPlaneInfo(const larocv::ImageMeta& meta)
  {
    if(meta.plane() >= _num_planes) {
      LAROCV_CRITICAL() << "Plane " << meta.plane() << " is out of bound!" << std::endl;
      throw larbys();
    }
    _time_comp_factor_v.at(meta.plane()) = meta.pixel_height();
    _wire_comp_factor_v.at(meta.plane()) = meta.pixel_width();
    
    _origin_v.at(meta.plane()).x = meta.origin().x;
    _origin_v.at(meta.plane()).y = meta.origin().y;

    _num_cols_v.at(meta.plane()) = meta.num_pixel_column();
    _num_rows_v.at(meta.plane()) = meta.num_pixel_row();
  }

  float LArPlaneGeo::tick_offset(const size_t plane) const
  { return _tick_offset_v.at(plane); }
  
  float LArPlaneGeo::x_offset(const size_t plane) const
  { return _tick_offset_v.at(plane) / _time_comp_factor_v.at(plane); }

  float LArPlaneGeo::x2col(float x, size_t plane) const
  { 
    auto tick = Cm2TriggerTick(x) + _trigger_tick;
    float min_x = _origin_v.at(plane).x;
    float max_x = min_x + _num_cols_v[plane] * _time_comp_factor_v[plane];

    if(tick < min_x || max_x < tick) {
      // std::stringstream ss;
      // ss << "3D X position " << x << " = tick " << tick
      // 	 << " out of bound " << min_x << " => " << max_x << std::endl;
      // throw larbys(ss.str());
      throw larbys();
    }

    return (tick - min_x) / _time_comp_factor_v[plane];
  }

  float LArPlaneGeo::yz2row(float y, float z, size_t plane) const
  {
    auto const wire = WireCoordinate(y,z,plane);
    auto const& min_y = _origin_v.at(plane).y;
    auto const& max_y = min_y + _num_rows_v[plane] * _wire_comp_factor_v[plane];
    if(wire < min_y || max_y < wire) {
      /*
      std::stringstream ss;
      ss << "3D (Y,Z) = (" << y << "," << z << ")"
	 << " corresponds to wire " << wire << " on plane " << plane 
	 << " ... out of bound " << min_y << " => " << max_y << std::endl;
      throw larbys(ss.str());
      */
      throw larbys();
    }
    return (wire - min_y) / _wire_comp_factor_v[plane];
  }

  float LArPlaneGeo::row2wire(float y, const size_t plane) const
  { return (y * _wire_comp_factor_v.at(plane) + _origin_v.at(plane).y); }

  float LArPlaneGeo::col2tick(float x, const size_t plane) const
  { return (x * _time_comp_factor_v.at(plane) + _origin_v.at(plane).x); }

  float LArPlaneGeo::col2cm  (float x, const size_t plane) const
  { return larocv::TriggerTick2Cm(col2tick(x,plane) - _trigger_tick); }

  float LArPlaneGeo::wire2row(float wire, const size_t plane) const
  { return (wire - _origin_v.at(plane).y) / _wire_comp_factor_v.at(plane); }

  float LArPlaneGeo::tick2col(float tick, const size_t plane) const
  { return (tick - _origin_v.at(plane).x) / _time_comp_factor_v.at(plane); }


  
  bool LArPlaneGeo::YZPoint(const geo2d::Vector<float>& pt0, const size_t plane0,
			    const geo2d::Vector<float>& pt1, const size_t plane1,
			    larocv::data::Vertex3D& result) const
  {
    result.Clear();
    
    if(std::fabs(pt0.x - pt1.x) > _xplane_tick_resolution) {
      //LAROCV_DEBUG() << "tick diff " << std::fabs(pt0.x - pt1.x) << " > " << _xplane_tick_resolution << std::endl;
      return false;
    }
    
    auto wire0 = row2wire(pt0.y, plane0);
    auto wire1 = row2wire(pt1.y, plane1);
    
    std::pair<double,double> wire1_range(kINVALID_DOUBLE,kINVALID_DOUBLE);
    try {
      wire1_range = larocv::OverlapWireRange(wire0, plane0, plane1);
    }
    catch(::larutil::LArUtilException& lare) {
      //LAROCV_WARNING() << lare.what() << "... bad YZ point estimate" << std::endl;
      return false;
    }
    
    if(wire1 < wire1_range.first || wire1_range.second < wire1) {
      //LAROCV_DEBUG()<<"wire "<<wire1<<"<"<<wire1_range.first<<" or "<<wire1<<">"<<wire1_range.second<<std::endl;
      return false;
    }

    larocv::IntersectionPoint((size_t)(wire0+0.5), plane0, (size_t)(wire1+0.5), plane1, result.y, result.z);

    result.x = larocv::TriggerTick2Cm( (pt0.x + pt1.x) / 2. * _time_comp_factor_v[plane0] +
				       _origin_v[plane1].x - _trigger_tick);

    result.num_planes = _num_planes;

    result.vtx2d_v.resize(_num_planes);
    for(size_t plane=0; plane<_num_planes; ++plane) {
      auto& vtx2d = result.vtx2d_v[plane].pt;
      double wire = kINVALID_DOUBLE;
      try {
	wire = larocv::WireCoordinate(result.y, result.z, plane);
      } catch(::larutil::LArUtilException& lare) {
	LAROCV_WARNING() << lare.what() << "... bad wire coordinate requested "
			 << "@ ("<<result.y<<","<<result.z<<") plane " << plane << std::endl;
	return false;
      }
      vtx2d.y = wire2row(wire,plane);
      vtx2d.x = (pt0.x + pt1.x)/2.;
    }
    return true;
  }
  
  bool LArPlaneGeo::YZPoint(const geo2d::Vector<float>& pt0, const size_t plane0,
			    const geo2d::Vector<float>& pt1, const size_t plane1) const
  {
    static data::Vertex3D tmp;
    return YZPoint(pt0,plane0,pt1,plane1,tmp);
  }
  
}
#endif
  
