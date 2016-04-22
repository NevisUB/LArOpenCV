#ifndef __RECOROIFILTER_CXX__
#define __RECOROIFILTER_CXX__

#include "RecoROIFilter.h"

namespace larcv{

  void RecoROIFilter::_Configure_(const ::fcllite::PSet &pset)
  {
    _max_rad_length = pset.get<double> ("MaxRadLength");    // minimum distance for ROI vertex to start point
    _numoutput      = pset.get<int>    ("TooManyClusters"); // how many is too many
    _strict_hit_cut = pset.get<int>    ("StrictHitCut"); // how many is too many
  }

  Cluster2DArray_t RecoROIFilter::_Process_(const larcv::Cluster2DArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  {
    Cluster2DArray_t oclusters; oclusters.reserve( clusters.size() );

    for( const auto& cluster : clusters ) {

      if ( cluster.roi.dist > _max_rad_length || cluster._numHits < _strict_hit_cut ) // start point distance to roi vertex
	continue;
 
      auto ocluster = cluster; // make a copy

      ocluster._startPt = ocluster.roi.startpt;
      ocluster._endPt   = ocluster.roi.endpt;

      ocluster._eigenVecFirst   = ocluster.roi.dir;
      
      oclusters.emplace_back( ocluster );
    }

    if ( oclusters.size() > _numoutput ) //to many clusters moving on to matching
      return Cluster2DArray_t();

    return oclusters;
  }

}
#endif
