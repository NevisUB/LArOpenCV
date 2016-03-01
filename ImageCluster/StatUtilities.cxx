#ifndef __IMAGECLUSTER_STATUTILITIES_CXX__
#define __IMAGECLUSTER_STATUTILITIES_CXX__

#include "StatUtilities.h"

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

  
  double stdevx( const Contour_t& contour )
  {

    if ( ! contour.size() ) return 0.0;
    
    double result = 0.0;
    auto    avgx  = meanx(contour);
    
    for(int i = 0; i < contour.size(); ++i)
      result += (contour.at(i).x - avgx)*(contour.at(i).x - avgx);
    
    return ::std::sqrt(result/((double)(contour.size())));
  }

  double stdevy( const Contour_t& contour )
  {

    if ( ! contour.size() ) return 0.0;
    
    double result = 0.0;
    auto    avgy  = meany(contour);
    
    for(int i = 0; i < contour.size(); ++i)
      result += (contour.at(i).y - avgy)*(contour.at(i).y - avgy);
    
    return ::std::sqrt(result/((double)(contour.size())));
  }

  
  double cov ( const Contour_t& contour )
  {
    
    if ( ! contour.size() ) return 0.0;
    
    double result = 0.0;
    auto   mean1  = meanx(contour);
    auto   mean2  = meany(contour);
    
    for(int i = 0; i < contour.size(); ++i)
      result += (contour.at(i).x - mean1)*(contour.at(i).y - mean2);
    
    return result/((double)contour.size());
  }
  
  double meanx( const Contour_t& contour )
  {
    if ( ! contour.size() ) return 0.0;
    double result = 0.0;
    
    for(int i = 0; i < contour.size(); ++i)
      result += contour.at(i).x;
    
    return result / ((double)( contour.size() ));
  }  

  double meany( const Contour_t& contour )
  {
    if ( ! contour.size() ) return 0.0;
    double result = 0.0;
    
    for(int i = 0; i < contour.size(); ++i)
      result += contour.at(i).y;
    
    return result / ((double)( contour.size() ));
  }  

  double roi_cov( const Contour_t& pts ) {
    
    double pearsons_r;
    auto co = cov   (pts);
    auto sx = stdevx(pts);
    auto sy = stdevy(pts);
    
    if ( sx != 0 && sy != 0) // trunk exists
      pearsons_r =  co / ( sx * sy );
    else
      pearsons_r = 0;
    
    return pearsons_r;

  }
  
  
}

#endif
