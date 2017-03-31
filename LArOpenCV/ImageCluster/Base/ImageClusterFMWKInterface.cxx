#ifndef IMAGECLUSTERFMWKINTERFACE_CXX
#define IMAGECLUSTERFMWKINTERFACE_CXX

#include "ImageClusterFMWKInterface.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryHelper.h"
#include "LArUtil/LArProperties.h"
#include "LArUtil/TimeService.h"
#include "LArOpenCV/Core/larbys.h"
namespace larocv {

  Config_t CreatePSetFromFile(std::string fname, std::string cfg_name)
  { return ::fcllite::CreatePSetFromFile(fname,cfg_name); }

  void IntersectionPoint(const size_t wire1, const size_t plane1,
			 const size_t wire2, const size_t plane2,
			 double& y, double& z)
  {
    auto g = ::larutil::Geometry::GetME();
    g->IntersectionPoint(wire1,wire2,plane1,plane2,y,z);
  }

  double WireCoordinate(const double y, const double z, const size_t plane)
  {
    double xyz[3];
    xyz[0]=0;
    xyz[1]=y;
    xyz[2]=z;
    auto g = ::larutil::Geometry::GetME();
    return g->WireCoordinate(xyz,plane);
  }

  int Get3DAxisN(const int& iplane0, const int& iplane1, const double& omega0, const double& omega1,
		 double& phi, double& theta)
  {
    return ::larutil::GeometryHelper::GetME()->Get3DAxisN(iplane0,iplane1, omega0, omega1, phi, theta);
  }

  std::pair<double,double> OverlapWireRange(const size_t wire, const size_t plane, const size_t target_plane)
  {
    double start[3];
    double end[3];
    auto g = ::larutil::Geometry::GetME();
    g->WireEndPoints(plane,wire,start,end);

    std::pair<double,double> res;
    
    res.first = g->WireCoordinate(start,target_plane);
    res.second = g->WireCoordinate(end,target_plane);

    if(res.first > res.second) std::swap(res.first,res.second);

    return res;
  }

  double TriggerTick2Cm(double tick)
  {
    static auto tpc_clock = larutil::TimeService::GetME()->TPCClock();
    static auto drift_velocity = larutil::LArProperties::GetME()->DriftVelocity();
    return (tick * tpc_clock.TickPeriod()) * drift_velocity;
  }

  double Cm2TriggerTick(double x)
  {
    static auto tpc_clock = larutil::TimeService::GetME()->TPCClock();
    static auto drift_velocity = larutil::LArProperties::GetME()->DriftVelocity();
    return x / drift_velocity / tpc_clock.TickPeriod();
  }

}
#endif
