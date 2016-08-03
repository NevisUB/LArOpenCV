// Author: Sudeep Pillai (spillai@csail.mit.edu)
// Modified for OpenCV 3.0: Kazuhiro Terao (kazuhiro@nevis.columbia.edu)
// Note: Stripped from Opencv (opencv/modules/python/src2/cv2.cpp)

# ifndef __OPECV_IMUTIL_CONVERT_H__
# define __OPECV_IMUTIL_CONVERT_H__

#include <Python.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

//#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <numpy/ndarrayobject.h>
#include "Core/Image2D.h"
#include "Core/Point2DArray.h"
static PyObject* opencv_error = 0;

namespace larocv {

  /**
     \namespace larocv::convert
     C++ namespace for a collection of Python<=>LArOpenCV C++ object conversion functions/classes
   */
  namespace convert {
    
    static int failmsg(const char *fmt, ...);
    
    class PyAllowThreads;
    
    class PyEnsureGIL;
    
#define ERRWRAP2(expr)				\
    try						\
      {						\
	PyAllowThreads allowThreads;		\
	expr;					\
      }						\
    catch (const cv::Exception &e)			\
      {							\
	PyErr_SetString(opencv_error, e.what());	\
	return 0;					\
      }
    
    //static PyObject* failmsgp(const char *fmt, ...);
    
    static size_t REFCOUNT_OFFSET = (size_t)&(((PyObject*)0)->ob_refcnt) +
      (0x12345678 != *(const size_t*)"\x78\x56\x34\x12\0\0\0\0\0")*sizeof(int);
    
    static inline PyObject* pyObjectFromRefcount(const int* refcount)
    {
      return (PyObject*)((size_t)refcount - REFCOUNT_OFFSET);
    }
    
    static inline int* refcountFromPyObject(const PyObject* obj)
    {
      return (int*)((size_t)obj + REFCOUNT_OFFSET);
    }
    
    
    class NumpyAllocator;
    
    enum { ARG_NONE = 0, ARG_MAT = 1, ARG_SCALAR = 2 };
    
    class NDArrayConverter
    {
    private:
      void init();
      public:
      NDArrayConverter();
      ::cv::Mat toMat(const PyObject* o);
      PyObject* toNDArray(const ::cv::Mat& mat);
      PyObject* toNDArray(const ::larocv::Image2D& img);
      PyObject* toNDArray(const ::larocv::Point2DArray& pt_v);
    };
  }
}
# endif
    
