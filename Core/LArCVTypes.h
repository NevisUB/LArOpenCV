#ifndef __LARCV_TYPES_H__
#define __LARCV_TYPES_H__

#include <vector>
#include <limits>
#include <climits>

/**
   \namespace larcaffe
   C++ namespace for developping LArTPC software interface to deep learning tool caffe
*/
namespace larcaffe {

  /**
     \struct Point2D
     Simple 2D point struct
  */
  struct Point2D {
    double x, y;
    Point2D(double xv=0, double yv=0) : x(xv), y(yv) {}
  };

}

/**
   \namespace larcv
   C++ namespace for developping LArTPC software interface to OpenCV based algorithm tools (LArOpenCV)
 */
namespace larcv {

  /// Used as an invalid value identifier for size_t
  static const size_t kINVALID_SIZE = std::numeric_limits<size_t>::max();
  /// Used as an invalid value identifier for int
  static const int    kINVALID_INT  = std::numeric_limits<int>::max();
  /// Stolen from larcaffe
  typedef ::larcaffe::Point2D Point2D;

}

#endif
