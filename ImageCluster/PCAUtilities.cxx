#ifndef __IMAGECLUSTER_PCAUTILITIES_CXX__
#define __IMAGECLUSTER_PCAUTILITIES_CXX__

#include "PCAUtilities.h"

namespace larcv {

  double stdev( ::std::vector<int>& data,
		size_t start, size_t end )
  {

    if ( (end - start) <=0 ) return 0.0;
    
    double result = 0.0;
    auto    avg   = mean(data,start,end);
    
    for(int i = start; i <= end; ++i)
      result += (data.at(i) - avg)*(data.at(i) - avg);
    
    return ::std::sqrt(result/((double)(end - start)));
  }

  
  double cov ( ::std::vector<int>& data1,
	       ::std::vector<int>& data2,
	       size_t start, size_t end )
  {
    
    if ( start == end ) return 0.0;
    
    double result = 0.0;
    auto   mean1  = mean(data1,start,end);
    auto   mean2  = mean(data2,start,end);
    
    for(int i = start; i <= end; ++i)
      result += (data1.at(i) - mean1)*(data2.at(i) - mean2);
    
    return result/((double)(end - start));
  }
  
  double mean( ::std::vector<int>& data,
	       size_t start, size_t end )
  {
    if ( start == end ) return 0.0;
    double result = 0.0;
    
    for(int i = start; i <= end; ++i)
      result += data.at(i);
    
    return result / ((double)( end - start ));
  }  



}

#endif
