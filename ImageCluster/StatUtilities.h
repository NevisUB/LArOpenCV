#ifndef __IMAGECLUSTER_STATUTILITIES_H__
#define __IMAGECLUSTER_STATUTILITIES_H__

#include "ImageClusterTypes.h"

namespace larcv {

  double stdev( ::std::vector<int>& data,
		size_t start, size_t end );
  
  double cov ( ::std::vector<int>& data1,
	       ::std::vector<int>& data2,
	       size_t start, size_t end );

  double mean( ::std::vector<int>& data,
	       size_t start, size_t end );

    
  double meanx( const Contour_t& contour );
  double meany( const Contour_t& contour );
  
  double stdevx( const Contour_t& contour );
  double stdevy( const Contour_t& contour );
  
  double cov ( const Contour_t& contour );
  
  double roi_cov( const Contour_t& pts );

}
#endif
