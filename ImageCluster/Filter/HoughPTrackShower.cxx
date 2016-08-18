#ifndef __HOUGHPTRACKSHOWER_CXX__
#define __HOUGHPTRACKSHOWER_CXX__

#include "HoughPTrackShower.h"

#include <array>

namespace larocv {

  void HoughPTrackShower::_Configure_(const ::fcllite::PSet &pset)
  {
    _hough_rho             = pset.get<double>("Rho");
    _hough_theta           = pset.get<double>("Theta");
    _hough_threshold       = pset.get<int>   ("Threshold");
    _hough_min_line_length = pset.get<double>("MinLineLength");
    _hough_max_line_gap    = pset.get<double>("MaxLineGap");
  }
  
  Cluster2DArray_t HoughPTrackShower::_Process_(const larocv::Cluster2DArray_t& clusters,
						const ::cv::Mat& img,
						larocv::ImageMeta& meta, larocv::ROI& roi)
  {
    
    //Easiest way is to run hough lines on specific cluster
    //rather than subset of ::cv::Mat img I think

    std::vector<std::vector<std::array<int,4> > > houghs_v;
    houghs_v.resize(clusters.size());
    
    for(int i = 0; i < clusters.size(); ++i) {
      
      auto& cluster = clusters[i];
      auto& houghs  = houghs_v[i];
      
      std::vector<::cv::Vec4i> lines;

      //works but subMat has different coordinates than img -.-
      
      auto& rect = cluster._boundingBox;
      ::cv::Mat subMat(img, rect);

      ::cv::HoughLinesP(subMat, lines, _hough_rho, _pi/180.0,
      			_hough_threshold, _hough_min_line_length, _hough_max_line_gap);
      
      houghs.resize(lines.size());
      
      for(int j = 0; j < lines.size(); ++j )
      	houghs[j] = {{ lines[j][0],  lines[j][1], lines[j][2], lines[j][3]}}; //2 braces?
      
    }
    
    return clusters;
  }

}
#endif
