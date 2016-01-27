/**
 * \file laropencv_base.h
 *
 * \ingroup Core
 * 
 * \brief Class definition file of laropencv_base
 *
 * @author Kazu - Nevis 2015
 */

/** \addtogroup Core

    @{*/

#ifndef __LAROPENCV_BASE_H__
#define __LAROPENCV_BASE_H__

#include <vector>
#include "laropencv_logger.h"

namespace larcv {
    
  /**
     \class laropencv_base
     Very base class of all everything else defined in this framework.
  */
  class laropencv_base {
    
  public:
    
    /// Default constructor
    laropencv_base(const std::string logger_name="laropencv_base")
      : _logger(nullptr)
    { _logger = &(::larcv::logger::get(logger_name)); }
    
    /// Default copy constructor
    laropencv_base(const laropencv_base &original) : _logger(original._logger) {}
    
    /// Default destructor
    virtual ~laropencv_base(){};
    
    /// Logger getter
    inline const larcv::logger& logger() const
    { return *_logger; }
    
    /// Verbosity level
    void set_verbosity(::larcv::msg::Level_t level)
    { _logger->set(level); }
    
  private:
    
    larcv::logger *_logger;   ///< logger
    
  };
}
#endif

/** @} */ // end of doxygen group
