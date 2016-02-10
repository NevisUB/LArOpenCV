#ifndef __IMAGECLUSTER_PCAUTILITIES_H__
#define __IMAGECLUSTER_PCAUTILITIES_H__

#include <vector>
#include <cmath>

namespace larcv {
    
  double mean ( ::std::vector<int>& data,
		size_t start, size_t end );
  
  double stdev( ::std::vector<int>& data,
		size_t start, size_t end );
  
  
  double cov  ( ::std::vector<int>& data1,
		::std::vector<int>& data2,
		size_t start, size_t end );
    
}
#endif
