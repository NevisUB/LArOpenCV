#ifndef __PCAOVERALL_CXX__
#define __PCAOVERALL_CXX__

#include "PCAOverall.h"
#include "PCAUtilities.h"

namespace larcv{

  void PCAOverall::_Configure_(const ::fcllite::PSet &pset)
  {
    _nMinInsideHits = pset.get<int>("NMinInsideHits");
  }

  Cluster2DArray_t PCAOverall::_Process_(const larcv::Cluster2DArray_t& clusters,
					 const ::cv::Mat& img,
					 larcv::ImageMeta& meta)
  {
    Cluster2DArray_t out_clusters = clusters;
    Contour_t all_locations;
    ::cv::findNonZero(img, all_locations); // get the non zero points

    for(const auto& loc: all_locations) {
      for(auto& ocluster : out_clusters) {
	if ( ::cv::pointPolygonTest(ocluster._contour,loc,false) < 0 )
	  continue;
	ocluster._insideHits.emplace_back(loc.x, loc.y);
      }
    }

    for(auto& ocluster : out_clusters) {

      ocluster._numHits = ocluster._insideHits.size();

      if (ocluster._numHits <= _nMinInsideHits) continue;
      
      Point2D e_vec_first,e_vec_second,e_center;
      double  e_val_first,e_val_second;
      
      pca_line(ocluster._insideHits,
	       e_vec_first,e_vec_second,
	       e_val_first,e_val_second,
	       e_center);

      ocluster._centerPt       = e_center;
      ocluster._eigenVecFirst  = e_vec_first;
      ocluster._eigenVecSecond = e_vec_second;
      ocluster._eigenValFirst  = e_val_first;
      ocluster._eigenValSecond = e_val_second;
      //ocluster._area           = ::cv::contourArea(ocluster._contour);
      //ocluster._perimeter      = ::cv::arcLength(ocluster._contour,1);
    }
    return out_clusters;
  }

}
#endif
