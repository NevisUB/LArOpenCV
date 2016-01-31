#ifndef __HOUGHPTRACKSHOWER_CXX__
#define __HOUGHPTRACKSHOWER_CXX__

#include "HoughPTrackShower.h"

namespace larcv {

  void HoughPTrackShower::_Configure_(const ::fcllite::PSet &pset)
  {
    _hough_rho             = pset.get<double>("HoughRho");
    // _hough_theta           = pset.get<double>("HoughTheta");
    _hough_threshold       = pset.get<int>   ("HoughThreshold");
    _hough_min_line_length = pset.get<double>("HoughMinLineLength");
    _hough_max_line_gap    = pset.get<double>("HoughMaxLineGap");

   
    
  }

  ContourArray_t HoughPTrackShower::_Process_(const larcv::ContourArray_t& clusters,
					      const ::cv::Mat& img,
					      larcv::ImageMeta& meta)
  {

    //Easiest way is to run hough lines on specific cluster
    //rather than subset of ::cv::Mat img I think

    std::vector<std::vector<std::array<int,4> > > houghs_v;
    houghs_v.resize(clusters.size());
    
    for(int i = 0; i < clusters.size(); ++i) {

      auto& cluster = clusters[i];
      auto& houghs  = houghs_v[i];

      std::vector<::cv::Vec4i> lines;

      ::cv::HoughLinesP(::cv::Mat(cluster), lines, _hough_rho, _pi/180.0,
      			_hough_threshold, _hough_min_line_length, _hough_max_line_gap);

      houghs.resize(lines.size());

      for(int j = 0; j < lines.size(); ++j )
	houghs[j] = {  lines[j][0],  lines[j][1],
		       lines[j][2],  lines[j][3] };      
      
    }

    std::swap(houghs_v,_houghs_v);
    
    return clusters;
  }

}
#endif
