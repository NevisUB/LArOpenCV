#ifndef __CDTRACKSHOWER_CXX__
#define __CDTRACKSHOWER_CXX__

#include "CDTrackShower.h"

namespace larcv{

  void CDTrackShower::_Configure_(const ::fcllite::PSet &pset)
  {}

  ContourArray_t CDTrackShower::_Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  { return ContourArray_t(); }

}
#endif
