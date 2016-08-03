#ifndef __LAROCV_CORE_POINT2DARRAY_H__
#define __LAROCV_CORE_POINT2DARRAY_H__

/**
 * \file Point2DArray.h
 *
 * \ingroup Core
 * 
 * \brief Class def header for a 2D point array holder
 *
 * @author tmw, kazu
 */

/** \addtogroup Core

    @{*/

#include <vector>
#include "LArCVTypes.h"
namespace larocv {

  namespace convert {
    class NDArrayConverter;
  }

  /**
     \class larocv::Point2DArray
     @brief An array of 2D points implemented as 1D std::vector<double> for I/O performance
     It is meant to be used to store larocv::Contour_t in a ROOT file but not yet implemented (ignore!)
   */
  class Point2DArray {
    friend class ::larocv::convert::NDArrayConverter;
    
  public:
    Point2DArray() : _data() {}
    ~Point2DArray() {}
    
    void resize    (const size_t i) { _data.resize(i*2);  }
    void reserve   (const size_t i) { _data.reserve(i*2); }
    void push_back (const double x, const double y)
    { _data.push_back(x); _data.push_back(y); }
    void set       (const size_t i, const double x, const double y)
    { _data[i] = x; _data[_data.size()/2+i] = y; }

    size_t size   ()               const { return _data.size() / 2;               }
    double x      (const size_t i) const { return _data[i];                       }
    double y      (const size_t i) const { return _data[_data.size()/2+i];        }
    Point2D point (const size_t i) const
    { return Point2D(_data[i], _data[_data.size()/2+i]); }

    const std::vector<double>& raw_data() { return _data; }
    
  private:

    std::vector<double> _data;

  };

}

#endif
/** @} */ // end of doxygen group 
