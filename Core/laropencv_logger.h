/**
 * \file laropencv_logger.h
 *
 * \ingroup Core
 * 
 * \brief Compiler macro def header for a larocv logger
 *
 * @author kazuhiro
 */

/** \addtogroup Core

    @{*/
#ifndef __LAROPENCV_LOGGER_H__
#define __LAROPENCV_LOGGER_H__

#include <cstdio>
#include <iostream>
#include <map>
#include "LArOCVTypes.h"

namespace larocv {
  /**
     \class larcaffe::logger
  */
  /**

     \class logger
     \brief Utility class used to show formatted message on the screen.
     A logger class for larcaffe. Simply shows a formatted colored message on a screen. \n
     A static getter method is provided to create a sharable logger instance (see larcaffe_base for useage). \n
  */
  
  class logger{
    
  public:
    
    /// Default constructor
  logger(const std::string& name="no_name")
    : _ostrm(&std::cout)
      , _name(name)
    {}
    
    /// Default destructor
    virtual ~logger(){};
    
  private:
    
    /// ostream
    std::ostream *_ostrm;
    
    /// Level
    msg::Level_t _level;
      
    /// Name
    std::string _name;
    
    /// Set of loggers
#ifndef __CINT__
#ifndef __CLING__
    static std::map<std::string,larocv::logger> *_logger_m;
#endif
#endif
    
  public:

    /// Logger's name
    const std::string& name() const { return _name; }

    /// Verbosity level setter
    void set(const msg::Level_t level) { _level = level; }

    /// Verbosity level getter
    msg::Level_t level() const { return _level; }

    /// Comparison operator for static collection of loggers
    inline bool operator<(const logger& rhs) const
    {
      if(_name < rhs.name()) return true;
      if(_name > rhs.name()) return false;
      return false;
    }
    
    /// Getter of a message instance 
    static logger& get(const std::string name);
    /*
    {
      if(!_logger_m) _logger_m = new std::map<std::string,larocv::logger>();
      auto iter = _logger_m->find(name);
      if(iter == _logger_m->end()) {
	iter = _logger_m->emplace(name,logger(name)).first;
	iter->second.set(msg::kNORMAL);
      }
      return iter->second;
    };
    */
    //
    // Verbosity level checker
    //
    inline bool debug   () const { return _level <= msg::kDEBUG;   }
    inline bool info    () const { return _level <= msg::kINFO;    }
    inline bool normal  () const { return _level <= msg::kNORMAL;  }
    inline bool warning () const { return _level <= msg::kWARNING; }
    inline bool error   () const { return _level <= msg::kERROR;   }
    /// Formatted message (simplest)
    std::ostream& send(const msg::Level_t) const;
    /// Formatted message (function name included)
    std::ostream& send(const msg::Level_t level,
		       const std::string& function ) const;
    /// Formatted message (function name + line number)
    std::ostream& send(const msg::Level_t level,
		       const std::string& function,
		       const unsigned int line_num ) const;
    /// Formatted message (function name + line number + file name)
    std::ostream& send(const msg::Level_t level,
		       const std::string& function,
		       const unsigned int line_num,
		       const std::string& file_name) const;
  };
  
  /* typedef ::larocv::logger logger; */
}
//
// Compiler macro for saving us from text typing
//
#define LAROCV_DEBUG(obj)    if(obj.logger().debug())    obj.logger().send(::larocv::msg::kDEBUG,    __FUNCTION__,__LINE__,__FILE__)
#define LAROCV_INFO(obj)     if(obj.logger().info())     obj.logger().send(::larocv::msg::kINFO,     __FUNCTION__,__LINE__         )
#define LAROCV_NORMAL(obj)   if(obj.logger().normal())   obj.logger().send(::larocv::msg::kNORMAL,   __FUNCTION__                  )
#define LAROCV_WARNING(obj)  if(obj.logger().warning())  obj.logger().send(::larocv::msg::kWARNING,  __FUNCTION__                  )
#define LAROCV_ERROR(obj)    if(obj.logger().error())    obj.logger().send(::larocv::msg::kERROR,    __FUNCTION__,__LINE__         )
#define LAROCV_CRITICAL(obj) obj.logger().send(::larocv::msg::kCRITICAL, __FUNCTION__,__LINE__,__FILE__)
#endif
/** @} */ // end of doxygen group 

