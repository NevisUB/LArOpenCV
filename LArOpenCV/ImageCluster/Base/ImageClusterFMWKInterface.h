#ifndef IMAGECLUSTERFMWKINTERFACE_H
#define IMAGECLUSTERFMWKINTERFACE_H

#define USE_LARLITE_PSET 1
//#define USE_LARCV_PSET 1

#ifdef USE_LARLITE_PSET
#include "BasicTool/FhiclLite/PSet.h"
#include "BasicTool/FhiclLite/FhiclLiteUtilFunc.h"
namespace larocv{
  typedef ::fcllite::PSet Config_t;
  Config_t CreatePSetFromFile(std::string fname, std::string cfg_name="noname");
}
#endif
#ifdef USE_LARCV_PSET
#include "core/Base/PSet.h"
namespace larocv{
  typedef ::larcv::PSet Config_t;
}
#endif

#include "Geo2D/Core/Vector.h"

namespace larocv {

  void IntersectionPoint(const size_t wire1, const size_t plane1,
			 const size_t wire2, const size_t plane2,
			 double& y, double& z);

  double WireCoordinate(const double y, const double z, const size_t plane);

  std::pair<double,double> OverlapWireRange(const size_t wire, const size_t plane, const size_t target_plane);

  int Get3DAxisN(const int& iplane0, const int& iplane1, const double& omega0, const double& omega1,
		 double& phi, double& theta);
  
  double TriggerTick2Cm(double tick);
}
#endif