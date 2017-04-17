#ifndef __LAROCV_TYPES_H__
#define __LAROCV_TYPES_H__

#include <limits>
#include <string>
#include <utility>
#include <vector>
#include <climits>

/**
   \namespace larocv
   C++ namespace for developping LArTPC software interface to OpenCV based algorithm tools (LArOpenCV)
 */
namespace larocv {

  struct Point2D {
    double x, y;
    Point2D(double xv=0, double yv=0) : x(xv), y(yv) {}
  };
  
  /// Used as an invalid value identifier for size_t
  static const size_t kINVALID_SIZE = std::numeric_limits<size_t>::max();
  /// Used as an invalid value identifier for int
  static const int    kINVALID_INT  = std::numeric_limits<int>::max();
  /// Used as an invalid vlaue identifier for float
  static const float  kINVALID_FLOAT   = std::numeric_limits<float>::max();
  /// Used as an invalid value identifier for double
  static const double kINVALID_DOUBLE  = std::numeric_limits<double>::max();
  /// Used as an invalid value identifier for double
  static const unsigned int kINVALID_UINT  = std::numeric_limits<unsigned int>::max();

  /// Namespace for larocv message related types
  
  namespace msg {

    enum Level_t { kDEBUG, kINFO, kNORMAL, kWARNING, kERROR, kCRITICAL, kMSG_TYPE_MAX };

    const std::string kStringPrefix[kMSG_TYPE_MAX] =
      {
	"     \033[94m[DEBUG]\033[00m ",  ///< kDEBUG message prefix
	"      \033[92m[INFO]\033[00m ",  ///< kINFO message prefix
	"    \033[95m[NORMAL]\033[00m ",  ///< kNORMAL message prefix
	"   \033[93m[WARNING]\033[00m ", ///< kWARNING message prefix
	"     \033[91m[ERROR]\033[00m ", ///< kERROR message prefix
	"  \033[5;1;33;41m[CRITICAL]\033[00m "  ///< kCRITICAL message prefix
      };
    ///< Prefix of message
  }
  
}

#endif
