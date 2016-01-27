/**
 * \file laropencv_logger.h
 *
 * \ingroup Core
 * 
 * \brief Compiler macro def header for a larcv logger
 *
 * @author kazuhiro
 */

/** \addtogroup Core

    @{*/
#ifndef __LAROPENCV_LOGGER_H__
#define __LAROPENCV_LOGGER_H__

#include "larcaffe_logger.h"
#include "LArCVTypes.h"
namespace larcv {
  /**
     \class larcaffe::logger
     Stealing from larcaffe::larcaffe_logger
  */
  typedef ::larcaffe::logger logger;
}
//
// Compiler macro for saving us from text typing
//
#define LARCV_DEBUG(obj)    if(obj.logger().debug())    obj.logger().send(::larcaffe::msg::kDEBUG,    __FUNCTION__,__LINE__,__FILE__)
#define LARCV_INFO(obj)     if(obj.logger().info())     obj.logger().send(::larcaffe::msg::kINFO,     __FUNCTION__,__LINE__         )
#define LARCV_NORMAL(obj)   if(obj.logger().normal())   obj.logger().send(::larcaffe::msg::kNORMAL,   __FUNCTION__                  )
#define LARCV_WARNING(obj)  if(obj.logger().warning())  obj.logger().send(::larcaffe::msg::kWARNING,  __FUNCTION__                  )
#define LARCV_ERROR(obj)    if(obj.logger().error())    obj.logger().send(::larcaffe::msg::kERROR,    __FUNCTION__,__LINE__         )
#define LARCV_CRITICAL(obj) if(obj.logger().critical()) obj.logger().send(::larcaffe::msg::kCRITICAL, __FUNCTION__,__LINE__,__FILE__)
#endif
/** @} */ // end of doxygen group 

