#ifndef __FILTERROI_CXX__
#define __FILTERROI_CXX__

#include "FilterROI.h"

namespace larocv {

  /// Global larocv::FilterROIFactory to register AlgoFactory
  static FilterROIFactory __global_FilterROIFactory__;

  void FilterROI::_Configure_(const ::fcllite::PSet &pset)
  {

  }
  
  Cluster2DArray_t FilterROI::_Process_(const larocv::Cluster2DArray_t& clusters,
						const ::cv::Mat& img,
						larocv::ImageMeta& meta, larocv::ROI& roi)
  {

    if ( meta.roi_cropped() ) return clusters;
    
    //lets remove clusters that are outside of the ROI region! seems simple enough
    Cluster2DArray_t oclusters;
    oclusters.reserve(clusters.size());

    std::vector<::cv::Point> roi_box;
    roi_box.resize(4);
    
    for (short i=0;i<4;++i) {
      auto rbii = roi.roibounds_in_image(meta,i);
      roi_box[i].x = rbii.x;
      roi_box[i].y = rbii.y;
    }
    
    bool outside=false;

    for( const auto & cluster : clusters ){

      outside=false;
      
      for (const auto& pt : cluster._contour ) {
	if ( ::cv::pointPolygonTest(roi_box,pt,false) < 0 )
	  { outside = true; break;}
      }
      
      if ( outside ) continue;
      
      oclusters.emplace_back(cluster);
    }
    
    return oclusters;
  }
  
}
#endif
