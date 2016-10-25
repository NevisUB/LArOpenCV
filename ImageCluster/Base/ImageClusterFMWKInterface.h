#ifndef IMAGECLUSTERFMWKINTERFACE_H
#define IMAGECLUSTERFMWKINTERFACE_H

#include "Core/Vector.h"

namespace larocv {

  void IntersectionPoint(const size_t wire1, const size_t plane1,
			 const size_t wire2, const size_t plane2,
			 double& y, double& z);

  double WireCoordinate(const double y, const double z, const size_t plane);

}
#endif
