#ifndef __LAROPENCV_BASE_H__
#define __LAROPENCV_BASE_H__

#include <vector>
#include "laropencv_logger.h"

namespace larocv {
    
  /**
     \class laropencv_base
     Very base class of all everything else defined in this framework.
  */
  class laropencv_base {
    
  public:
    
    /// Default constructor
    laropencv_base(const std::string logger_name="laropencv_base")
      : _logger(nullptr)
    { _logger = &(::larocv::logger::get(logger_name)); }
    
    /// Default copy constructor
    laropencv_base(const laropencv_base &original) : _logger(original._logger) {}
    
    /// Default destructor
    virtual ~laropencv_base(){};
    
    /// Logger getter
    inline const larocv::logger& logger() const
    { return *_logger; }
    
    /// Verbosity level
    void set_verbosity(::larocv::msg::Level_t level)
    { _logger->set(level); }
    
  private:
    
    larocv::logger *_logger;   //! do not serialize to ROOT file
    
  };
}
#endif

/** @} */ // end of doxygen group
