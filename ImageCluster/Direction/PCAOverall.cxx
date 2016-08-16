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

      auto& reco = ocluster.reco;
      reco.dir   = e_vec_first;
    }

    /// Debug mode is off, return the result
    if (!meta.debug()) return out_clusters;

    //***************************************************
    //***Debug Mode
    
    std::stringstream ss1;

    ::larlite::user_info uinfo{};

    for(size_t i = 0; i < out_clusters.size(); ++i){

      const auto& imgclus = out_clusters[i];

      ss1  << Name() << "_" << meta.plane() << "_dir_" << i;

      double dir_x = imgclus.reco.dir.x;
      double dir_y = imgclus.reco.dir.y;

      uinfo.append(ss1.str(),dir_x);
      uinfo.append(ss1.str(),dir_y);

      ss1.str(std::string());
      ss1.clear();
    }
      
    meta.ev_user()->emplace_back(uinfo);
    
    return out_clusters;
  }

}
#endif
