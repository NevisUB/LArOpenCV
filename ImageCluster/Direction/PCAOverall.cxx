#ifndef __PCAOVERALL_CXX__
#define __PCAOVERALL_CXX__

#include "PCAOverall.h"
#include "PCAUtilities.h"

namespace larocv{

  void PCAOverall::_Configure_(const ::fcllite::PSet &pset)
  { }

  Cluster2DArray_t PCAOverall::_Process_(const larocv::Cluster2DArray_t& clusters,
					 const ::cv::Mat& img,
					 larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    Cluster2DArray_t out_clusters = clusters;

    for(auto& ocluster : out_clusters) {

      Point2D e_vec_first,e_vec_second,e_center;
      double  e_val_first,e_val_second;

      pca_line(ocluster._insideHits,
	       e_vec_first,e_vec_second,
	       e_val_first,e_val_second,
	       e_center);
      
      ocluster._eigenVecFirst = e_vec_first;
      ocluster._centerPt = e_center;
      
      auto& reco = ocluster.reco;
      reco.dir   = e_vec_first;
    }


    return out_clusters;
  }

}
#endif
