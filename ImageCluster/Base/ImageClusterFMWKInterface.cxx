#ifndef IMAGECLUSTERFMWKINTERFACE_CXX
#define IMAGECLUSTERFMWKINTERFACE_CXX

#include "ImageClusterFMWKInterface.h"
#include "LArUtil/Geometry.h"
#include "Core/larbys.h"
namespace larocv {

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

}
#endif
