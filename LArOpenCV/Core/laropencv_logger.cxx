#ifndef __LAROCVLOGGER_CXX__
#define __LAROCVLOGGER_CXX__

#include "laropencv_logger.h"

namespace larocv {

  std::map<std::string,logger> *logger::_logger_m = nullptr;

  logger* logger::_shared_logger = nullptr;
  
  std::ostream& logger::send(const msg::Level_t level) const
  {
    (*_ostrm)  << msg::kStringPrefix[level].c_str()
	       << "\033[0m ";
    return (*_ostrm);
  }
  
  std::ostream& logger::send(const msg::Level_t level,
			     const std::string& function ) const
  {
    auto& strm(send(level));
    strm << "\033[94m<" << _name << "::" << function.c_str() << ">\033[00m ";
    return strm;
  }
  
  std::ostream& logger::send(const msg::Level_t level,
			     const std::string& function,
			     const unsigned int line_num ) const
  {
    auto& strm(send(level));
    strm << "\033[94m<" << _name << "::" << function.c_str() << "::L" << line_num << ">\033[00m ";
    return strm;
  }
  
  std::ostream& logger::send(const msg::Level_t level,
			     const std::string& function,
			     const unsigned int line_num,
			     const std::string& file_name) const
  {
    auto& strm(send(level,function));
    strm << file_name.c_str() << "::L" << line_num << " ";
    return strm;
  }
}

#endif
