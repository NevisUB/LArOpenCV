#ifndef __HOUGHTRACKSHOWER_CXX__
#define __HOUGHTRACKSHOWER_CXX__

#include "HoughTrackShower.h"

namespace larcv{

  void HoughTrackShower::_Configure_(const ::fcllite::PSet &pset)
  {}

  ContourArray_t HoughTrackShower::_Process_(const larcv::ContourArray_t& clusters,
					     const ::cv::Mat& img,
					     larcv::ImageMeta& meta)
  {
    
    
    
    
    return clusters;
  }

}
#endif
