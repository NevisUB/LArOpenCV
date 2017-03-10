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
    _origin_v.resize(_num_planes);
  }

  void LArPlaneGeo::Configure(const Config_t &pset)
  {
    this->set_verbosity((msg::Level_t)(pset.get<unsigned short>("Verbosity", (unsigned short)(this->logger().level()))));
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
    LAROCV_DEBUG() << " Plane " << meta.plane()
		   << " wire comp factor: " << _wire_comp_factor_v[meta.plane()]
		   << " time comp factor: " << _time_comp_factor_v[meta.plane()]
		   << std::endl;
    
    _origin_v.at(meta.plane()).x = meta.origin().x;
    _origin_v.at(meta.plane()).y = meta.origin().y;
  }

  float LArPlaneGeo::tick_offset(const size_t plane) const
  { return _tick_offset_v.at(plane); }

  float LArPlaneGeo::x_offset(const size_t plane) const
  { return _tick_offset_v.at(plane) / _time_comp_factor_v.at(plane); }

  float LArPlaneGeo::y2wire(float y, const size_t plane) const
  { return (y * _wire_comp_factor_v.at(plane) + _origin_v.at(plane).y); }

  float LArPlaneGeo::x2tick(float x, const size_t plane) const
  { return (x * _time_comp_factor_v.at(plane) + _origin_v.at(plane).x); }

  float LArPlaneGeo::x2cm  (float x, const size_t plane) const
  { return larocv::TriggerTick2Cm(x2tick(x,plane) - _trigger_tick); }

  float LArPlaneGeo::wire2y(float wire, const size_t plane) const
  { return (wire - _origin_v.at(plane).y) / _wire_comp_factor_v.at(plane); }

  float LArPlaneGeo::tick2x(float tick, const size_t plane) const
  { return (tick - _origin_v.at(plane).x) / _time_comp_factor_v.at(plane); }

  bool LArPlaneGeo::YZPoint(const geo2d::Vector<float>& pt0, const size_t plane0,
			    const geo2d::Vector<float>& pt1, const size_t plane1,
			    larocv::data::Vertex3D& result) const
  {
    result.Clear();

    if(std::fabs(pt0.x - pt1.x) > _xplane_tick_resolution) return false;

    auto wire0 = y2wire(pt0.y, plane0);
    auto wire1 = y2wire(pt1.y, plane1);

    std::pair<double,double> wire1_range(kINVALID_DOUBLE,kINVALID_DOUBLE);
    try {
      wire1_range = larocv::OverlapWireRange(wire0, plane0, plane1);
    }
    catch(::larutil::LArUtilException& lare) {
      LAROCV_WARNING() << lare.what() << "... bad YZ point estimate" << std::endl;
      return false;
    }
    
    if(wire1 < wire1_range.first || wire1_range.second < wire1) return false;

    larocv::IntersectionPoint((size_t)(wire0+0.5), plane0, (size_t)(wire1+0.5), plane1, result.y, result.z);

    result.x = larocv::TriggerTick2Cm( (pt0.x + pt1.x) / 2. * _time_comp_factor_v[plane0] +
				       _origin_v[plane1].x - _trigger_tick);

    result.num_planes = _num_planes;

    result.vtx2d_v.resize(_num_planes);
    for(size_t plane=0; plane<_num_planes; ++plane) {
      auto& vtx2d = result.vtx2d_v[plane].pt;
      auto wire = larocv::WireCoordinate(result.y, result.z, plane);
      vtx2d.y = wire2y(wire,plane);
      vtx2d.x = (pt0.x + pt1.x)/2.;
    }
    return true;
  }
  bool LArPlaneGeo::YZPoint(const geo2d::Vector<float>& pt0, const size_t plane0,
			    const geo2d::Vector<float>& pt1, const size_t plane1) const
  {
    data::Vertex3D tmp;
    return YZPoint(pt0,plane0,pt1,plane1,tmp);
  }


  float LArPlaneGeo::Overlap(const geo2d::VectorArray<float>& pts0_v, const size_t plane0,
			     const geo2d::VectorArray<float>& pts1_v, const size_t plane1,
			     bool overcover) const
  {
    
    //get the smallest set of points
    const geo2d::VectorArray<float>* max_pts_v=nullptr;
    const geo2d::VectorArray<float>* min_pts_v=nullptr;
    size_t min_plane=kINVALID_SIZE;
    size_t max_plane=kINVALID_SIZE;
    if (pts0_v.size()>=pts1_v.size()) {
      max_pts_v = &pts0_v;
      min_pts_v = &pts1_v;
      max_plane = plane0;
      min_plane = plane1;
    }
    else {
      max_pts_v = &pts1_v;
      min_pts_v = &pts0_v;
      max_plane = plane1;
      min_plane = plane0;
    }

    float overlap=0.0;
    std::vector<bool> used_v(max_pts_v->size(),false);
    for(size_t min_id=0;min_id<min_pts_v->size();++min_id) {
      const auto& min_pt = (*min_pts_v)[min_id];
      for(size_t max_id=0;max_id<max_pts_v->size();++max_id) {
	if (used_v[max_id]) continue;
	const auto& max_pt = (*max_pts_v)[max_id];
	if (YZPoint(min_pt,min_plane,max_pt,max_plane)) {
	  overlap+=1.0;
	  used_v[max_id] = true;
	  if (!overcover) break;
	}
      }
    }

    // it's possible for overlap to be greater than 1...
    // a point on the first plane may overlap 2 points on the second?
    
    overlap /= (float)min_pts_v->size();
    
    LAROCV_DEBUG() << "Calculated overlap " << overlap << std::endl;
    return overlap;
  }
  
}
#endif
  
