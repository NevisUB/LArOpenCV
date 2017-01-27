#ifndef PIRANGE_CXX
#define PIRANGE_CXX

#include "PiRange.h"
#include "LArOpenCV/Core/larbys.h"
#include <cmath>
namespace larocv {

  void PiRange::SetAngle(double angle, double range_lo, double range_hi)
  {
    _angle    = angle;
    if(range_lo < 0. || range_lo > 180.) {
      LAROCV_CRITICAL() << "Lo range = " << range_lo << " (must be [0,180] range)" << std::endl;
      throw larbys();
    }

    if(range_hi < 0. || range_hi > 180.) {
      LAROCV_CRITICAL() << "Lo range = " << range_hi << " (must be [0,180] range)" << std::endl;
      throw larbys();
    }

    while(_angle>360) { _angle -= 360; }
    while(_angle<0  ) { _angle += 360; }

    _angle_hi = _angle + range_hi;
    _angle_lo = _angle - range_lo;
    while(_angle_hi>360) { _angle_hi -= 360; }
    while(_angle_hi<0  ) { _angle_hi += 360; }
    while(_angle_lo>360) { _angle_lo -= 360; }
    while(_angle_lo<0  ) { _angle_lo += 360; }
  }

  bool PiRange::CloserEdge(double angle) const
  {
    while(angle>360) { angle -= 360.; }
    while(angle<0  ) { angle += 360.; }
    double dangle_hi = std::fabs(_angle_hi - angle);
    if( dangle_hi > 180. ) dangle_hi = 360. - dangle_hi;

    double dangle_lo = std::fabs(_angle_lo - angle);
    if( dangle_lo > 180. ) dangle_lo = 360. - dangle_lo;

    return dangle_hi < dangle_lo;
  }
  
  bool PiRange::Inside(double angle) const
  {
    while(angle>360) { angle -= 360.; }
    while(angle<0  ) { angle += 360.; }
    if(CloserEdge(angle)) {
      double angle_rotate  = 360 - _angle_hi;
      double rotated_angle = angle + angle_rotate;
      if( rotated_angle > 360. ) rotated_angle -= 360.;
      LAROCV_DEBUG() << "Close to HI angle (" << AngleLo() << "," << AngleHi()
		     << ") @ " << angle << " ... " << (rotated_angle > 180. ? "inside" : "outside")
		     << std::endl;
      return rotated_angle >180.;
    }else{
      double angle_rotate  = 360. - _angle_lo;
      double rotated_angle = angle + angle_rotate;
      if( rotated_angle > 360. ) rotated_angle -= 360.;
      LAROCV_DEBUG() << "Close to LO angle (" << AngleLo() << "," << AngleHi()
		     << ") @ " << angle << " ... " << (rotated_angle < 180. ? "inside" : "outside")
		     << std::endl;
      return rotated_angle < 180.;
    }
  }

}
#endif
