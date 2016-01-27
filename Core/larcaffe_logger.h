/**
 * \file larcaffe_logger.h
 *
 * \ingroup LArCaffe
 * 
 * \brief logger utility class definition header file.
 *
 * @author Kazu - Nevis 2015
 */

/** \addtogroup LArCaffe

    @{*/
#ifndef __LARCAFFELOGGER_H__
#define __LARCAFFELOGGER_H__

#include <cstdio>
#include <iostream>
#include <map>
//#include "LArCaffe/LArCaffeTypes.h"
#include "LArCaffeTypes.h"

namespace larcaffe {

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
    static std::map<std::string,larcaffe::logger> *_logger_m;
    
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
    static logger& get(const std::string name)
    {
      if(!_logger_m) _logger_m = new std::map<std::string,larcaffe::logger>();
      auto iter = _logger_m->find(name);
      if(iter == _logger_m->end()) {
	iter = _logger_m->emplace(name,logger(name)).first;
	iter->second.set(msg::kNORMAL);
      }
      return iter->second;
    };
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
}
//
// Compiler macro for saving us from text typing
//
#define LARCAFFE_DEBUG(obj) obj.send(::larcaffe::msg::kDEBUG,__FUNCTION__,__LINE__,__FILE__)
#define LARCAFFE_INFO(obj) obj.send(::larcaffe::msg::kINFO,__FUNCTION__,__LINE__)
#define LARCAFFE_NORMAL(obj) obj.send(::larcaffe::msg::kNORMAL,__FUNCTION__)
#define LARCAFFE_WARNING(obj) obj.send(::larcaffe::msg::kWARNING,__FUNCTION__)
#define LARCAFFE_ERROR(obj) obj.send(::larcaffe::msg::kERROR,__FUNCTION__,__LINE__)
#define LARCAFFE_CRITICAL(obj) obj.send(::larcaffe::msg::kCRITICAL,__FUNCTION__,__LINE__,__FILE__)
  
/** @} */ // end of doxygen group logger
#endif
