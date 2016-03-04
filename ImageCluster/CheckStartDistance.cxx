#ifndef __CHECKSTARTDISTANCE_CXX__
#define __CHECKSTARTDISTANCE_CXX__

#include "CheckStartDistance.h"
#include "PCAUtilities.h"

namespace larcv{

  void CheckStartDistance::_Configure_(const ::fcllite::PSet &pset)
  {
    _max_start_d = pset.get<double>("MaxStartDistance");

    _w = 0.0557;
    _h = 0.3;
  }

  Cluster2DArray_t CheckStartDistance::_Process_(const larcv::Cluster2DArray_t& clusters,
						 const ::cv::Mat& img,
						 larcv::ImageMeta& meta)
  {
    
       
    Cluster2DArray_t oclusters; oclusters.reserve( clusters.size() );

    std::map<int,bool> keep; for(int i=0;i<clusters.size();++i) keep[i] = false;
    
    for( unsigned i = 0 ; i < clusters.size(); ++i ) {
      for( unsigned j = 0 ; j < clusters.size(); ++j ) {

	if ( i == j ) continue;
	
	auto& s1 = clusters[i]._startPt;
	auto& s2 = clusters[j]._startPt;

	auto d = dist(s1.x * _w, s2.x * _w,
		      s1.y * _h, s2.y * _h);
	if ( d <= _max_start_d ) { keep[i] = true; keep[j] = true; }
	  
      }
    }

    for ( auto& k : keep )
      {  if ( k.second ) oclusters.emplace_back(clusters[k.first]); }

    return oclusters;
  }

}
#endif
