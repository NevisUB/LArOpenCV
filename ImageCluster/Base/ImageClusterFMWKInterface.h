#ifndef IMAGECLUSTERFMWKINTERFACE_H
#define IMAGECLUSTERFMWKINTERFACE_H

#include "Core/Vector.h"

namespace larocv {

  void IntersectionPoint(const size_t wire1, const size_t plane1,
			 const size_t wire2, const size_t plane2,
			 double& y, double& z);

  double WireCoordinate(const double y, const double z, const size_t plane);

  std::pair<double,double> OverlapWireRange(const size_t wire, const size_t plane, const size_t target_plane);

  double TriggerTick2Cm(double tick);
}
#endif
