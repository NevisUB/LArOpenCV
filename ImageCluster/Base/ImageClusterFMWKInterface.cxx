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

}
#endif
