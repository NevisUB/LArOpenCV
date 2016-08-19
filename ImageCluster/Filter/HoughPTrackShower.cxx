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

    
    // I thought about running hough line finder cluster-by-cluster but thats
    // certainly not useful. I might as well do regression if that's the case.
    // I think I would rather look for overall line structure in the (possibly) cropped image,
    // this may help me idenfity clusters which are possibly connected to one another

    ::std::vector<::cv::Vec4i> lines;
    ::cv::HoughLinesP(img,lines,_hough_rho,_pi/180.0,
		      _hough_threshold,_hough_min_line_length,
		      _hough_max_line_gap);
    
    //save the lines for now w/ debug mode


    /// Debug mode is off, return the result
    if ( ! meta.debug() ) return clusters;
    
    //***************************************************
    //***Debug Mode

    std::stringstream ss1,ss2,ss3,ss4;

    ::larlite::user_info uinfo{};
    ss1 << "Algo_"<<Name()<<"_Plane_"<<meta.plane()<<"_clusters";
    uinfo.store("ID",ss1.str());


    ss1.str(std::string());
    ss1.clear();

    uinfo.store("N_lines",(int)lines.size());
    
    for(uint i=0;i<lines.size();++i) {

      auto& line = lines[i];

      ss1 << "line_" << i << "_x1";
      ss2 << "line_" << i << "_y1";
      ss3 << "line_" << i << "_x2";
      ss4 << "line_" << i << "_y2";
      
      uinfo.store(ss1.str(),meta.XtoTimeTick(lines[i][0]));
      uinfo.store(ss2.str(),meta.YtoWire    (lines[i][1]));
      uinfo.store(ss3.str(),meta.XtoTimeTick(lines[i][2]));
      uinfo.store(ss4.str(),meta.YtoWire    (lines[i][3]));
      ss1.str(std::string()); ss1.clear();
      ss2.str(std::string()); ss2.clear();
      ss3.str(std::string()); ss3.clear();
      ss4.str(std::string()); ss4.clear();
      
    }

    meta.ev_user()->emplace_back(uinfo);
    
    // std::vector<std::vector<std::array<int,4> > > houghs_v;
    // houghs_v.resize(clusters.size());
    
    // for(int i = 0; i < clusters.size(); ++i) {
      
    //   auto& cluster = clusters[i];
      
    //   //works... but subMat has different coordinates than img -.-
      
    //   auto& rect = cluster._boundingBox;
    //   ::cv::Mat subMat(img, rect);

    //   houghs.resize(lines.size());
      
    //   for(int j = 0; j < lines.size(); ++j )
    //   	houghs[j] = {{ lines[j][0],  lines[j][1], lines[j][2], lines[j][3]}}; //2 braces?
      
    // }

    
    return clusters;
  }

}
#endif
