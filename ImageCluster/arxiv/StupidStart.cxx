#ifndef __STUPIDSTART_CXX__
#define __STUPIDSTART_CXX__

#include "StupidStart.h"

namespace larcv{

  void StupidStart::_Configure_(const ::fcllite::PSet &pset)
  {
    std::cout << "Careful, you have subjected yourself to stupidity. throw std::exception()!!!\n";
    std::cout << "         you have subjected yourself to stupidity. throw std::exception()!!!\n";
    std::cout << "         you have subjected yourself to stupidity.\n";
    std::cout << "                            yourself to stupidity.\n";
    std::cout << "         you                yourself to stupidity.\n";
    std::cout << "         you                            stupidity.\n";
    std::cout << "         you                            stupid   .\n";
			    
  }

  Cluster2DArray_t StupidStart::_Process_(const larcv::Cluster2DArray_t& clusters,
					const ::cv::Mat& img,
					larcv::ImageMeta& meta)
  {
    larcv::Cluster2DArray_t oclusters = clusters;
    
    //Get PIXEL with lowest wire, lowest tick --> start pt
    //Get PIXEL with highest wire, highest tick --> end pt


    for ( auto& ocluster : oclusters ) {

      auto min_w = 99999; auto min_t = 99999;
      auto max_w = 0;     auto max_t = 0;
      
      for ( const auto& h : ocluster._insideHits ) {
	
	if ( h.x > max_t ) max_t = h.x;
	if ( h.x < min_t ) min_t = h.x;
	if ( h.y > max_w ) max_w = h.y;
	if ( h.y < min_w ) min_w = h.y;
      
      }

      // std::cout << "(min_w,min_t): (" << min_w << "," << min_t << "\n";
      // std::cout << "(max_w,max_t): (" << max_w << "," << max_t << "\n";
      ocluster._startPt = Point2D(min_w,min_t);
      ocluster._endPt   = Point2D(max_w,max_t);
      
    }

    
    return oclusters;

    
  }

}
#endif
