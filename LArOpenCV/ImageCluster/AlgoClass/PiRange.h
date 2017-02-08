/**
 * \file PiRange.h
 *
 * \ingroup AlgoClass
 * 
 * \brief Class def header for a class PiRange
 *
 * @author kazuhiro
 */

/** \addtogroup AlgoClass

    @{*/
#ifndef PIRANGE_H
#define PIRANGE_H

#include "LArOpenCV/Core/laropencv_base.h"
namespace larocv {
  /**
     \class PiRange
     User defined class PiRange ... these comments are used to generate
     doxygen documentation!
  */
  class PiRange : public laropencv_base {
    
  public:
    
    /// Default constructor
    PiRange() : laropencv_base("PiRange") { _angle = _angle_hi = _angle_lo = 0;}
    
    /// Default destructor
    ~PiRange(){}

    void SetAngle(double angle, double range_lo, double range_hi);

    bool Inside(double angle) const;

    double Angle()   const { return _angle;    }
    double AngleHi() const { return _angle_hi; }
    double AngleLo() const { return _angle_lo; }

    /// returns true if closer to "hi" angle, fale for "lo" angle
    bool   CloserEdge(double angle) const;
    
  private:
    double _angle;
    double _angle_hi;
    double _angle_lo;
  };
}

#endif
/** @} */ // end of doxygen group 

