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

    Cluster2DArray_t out_clusters; out_clusters.resize(clusters.size());
    
    //for(auto& cluster : clusters) {
    for(unsigned i = 0; i < clusters.size(); ++i) {

      auto& cluster = clusters[i];
      auto& contour = cluster._contour;

      //make a copy into the ouptut
      auto ocluster = clusters[i];
      
      Contour_t all_locations;
      ::cv::findNonZero(img, all_locations); // get the non zero points

      for(const auto& loc : all_locations) {
	if ( ::cv::pointPolygonTest(contour,loc,false) < 0 )
	  continue;
	
	ocluster._insideHits.emplace_back(loc.x, loc.y);
	
      }

      ocluster._numHits = cluster._insideHits.size();

      if (ocluster._numHits <= _nMinInsideHits) continue;
      
      Point2D e_vec_first,e_vec_second,e_center;
      pca_line(ocluster._insideHits,
	       e_vec_first,e_vec_second,e_center);

      ocluster._centerPt      = e_center;
      ocluster._eigenVecFirst  = e_vec_first;
      ocluster._eigenVecSecond = e_vec_second;

    }
    return out_clusters;
  }

}
#endif
