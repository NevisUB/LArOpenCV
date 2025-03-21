# ifndef __COVERSION_OPENCV_H__
# define __COVERSION_OPENCV_H__

#include <Python.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "numpy/ndarrayobject.h"

#define NUMPY_IMPORT_ARRAY_RETVAL

namespace imutil {
  namespace convert {

    static PyObject* opencv_error = 0;
    
    static int failmsg(const char *fmt, ...);
    
    class PyAllowThreads
    {
    public:
      PyAllowThreads() : _state(PyEval_SaveThread()) {}
      ~PyAllowThreads()
      {
	PyEval_RestoreThread(_state);
      }
    private:
      PyThreadState* _state;
    };
    
    class PyEnsureGIL
    {
    public:
      PyEnsureGIL() : _state(PyGILState_Ensure()) {}
      ~PyEnsureGIL()
      {
	PyGILState_Release(_state);
      }
    private:
      PyGILState_STATE _state;
    };
    
#define ERRWRAP2(expr)				\
    try						\
      {						\
	PyAllowThreads allowThreads;		\
	expr;					\
      }						\
    catch (const ::cv::Exception &e)		\
      {						\
	PyErr_SetString(opencv_error, e.what());	\
	return 0;					\
      }
    
    static PyObject* failmsgp(const char *fmt, ...);
    
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
    
    class NDArrayConverter;
    
    class NumpyAllocator : public ::cv::MatAllocator
    {
      friend class NDArrayConverter;
    private:
      NumpyAllocator() {}
      ~NumpyAllocator() {}
      
      static NumpyAllocator& get() {
	if(!_me) _me = new NumpyAllocator;
	return *_me;
      }
      
      void allocate(int dims, const int* sizes, int type, int*& refcount,
		    uchar*& datastart, uchar*& data, size_t* step);
      void deallocate(::cv::UMatData* data) const = 0;
      //void deallocate(int* refcount, uchar*, uchar*);
      virtual UMatData* allocate(int dims, const int* sizes, int type,
				 void* data, size_t* step, int flags, UMatUsageFlags usageFlags) const = 0;
      //virtual bool allocate(UMatData* data, int accessflags, UMatUsageFlags usageFlags) const = 0;

      //TODO need to implement this
      bool allocate(::cv::UMatData* data, int accessflags, ::cv::UMatUsageFlags usageFlags) const { return false; }
      
      static NumpyAllocator *_me;
    };
    
    enum { ARG_NONE = 0, ARG_MAT = 1, ARG_SCALAR = 2 };
    
    class NDArrayConverter
    {
    private:
      void init();
    public:
      NDArrayConverter();
      ::cv::Mat toMat(const PyObject* o);
      PyObject* toNDArray(const ::cv::Mat& mat);
    };
  } // namespace convert
} // namespace imutil
# endif
