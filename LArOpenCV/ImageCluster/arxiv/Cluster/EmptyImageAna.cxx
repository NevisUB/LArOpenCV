#ifndef __EMPTYIMAGEANA_CXX__
#define __EMPTYIMAGEANA_CXX__

#include "EmptyImageAna.h"

namespace larocv{

  static EmptyImageAnaFactory __global_EmptyImageAnaFactory__;

  void EmptyImageAna::_Configure_(const Config_t &pset)
  { }

  void EmptyImageAna::_Process_(const Cluster2DArray_t& clusters,
				::cv::Mat& img,
				ImageMeta& meta,
				ROI& roi)
  {  return; }
    

}
#endif
