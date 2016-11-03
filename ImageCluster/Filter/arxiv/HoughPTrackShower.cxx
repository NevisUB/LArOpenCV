#ifndef __HOUGHPTRACKSHOWER_CXX__
#define __HOUGHPTRACKSHOWER_CXX__

#include "HoughPTrackShower.h"

#include <array>

namespace larocv {

  /// Global larocv::HoughPTrackShowerFactory to register AlgoFactory
  static HoughPTrackShowerFactory __global_HoughPTrackShowerFactory__;

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
    Cluster2DArray_t oclusters;
    oclusters.reserve(clusters.size());
    
    // I thought about running hough line finder cluster-by-cluster but thats
    // certainly not useful. I might as well do regression if that's the case.
    // I think I would rather look for overall line structure in the (possibly) cropped image,
    // this may help me idenfity clusters which are possibly connected to one another

    ::std::vector<::cv::Vec4i> lines;
    ::cv::HoughLinesP(img,lines,_hough_rho,_pi/180.0,
		      _hough_threshold,_hough_min_line_length,
		      _hough_max_line_gap);
    
    //save the lines for now w/ debug mode


    // How to decide to remove cluster or not? Can check for a few things, first
    // criteria that come to mind is to remove clusters that have a hough line inside.
    // What do we actually check? If the (x1,y1) or (x2,y2) is inside. Next we should
    // check if

    //no lines found, just return 
    if ( ! lines.size() ) return clusters;
    
    for(const auto& cluster : clusters){

      size_t i = 0; 
      for ( ; i < lines.size(); ++i ) {

	const auto& line = lines[i];
		
	//check if the first point is inside
	if ( ::cv::pointPolygonTest(cluster._contour,::cv::Point(line[0],line[1]),false) >= 0 ) break;

	//check if the second point is inside
	if ( ::cv::pointPolygonTest(cluster._contour,::cv::Point(line[2],line[3]),false) >= 0 ) break;

	//check if this point intersects this contour
	size_t j = 0;
	for( ; j < cluster._contour.size()-1; ++j)
	  {
	    // std::cout << "A: " << cluster._contour[j] << cluster._contour[j+1] << std::endl;
	    // std::cout << "B: " << line[0] << "," << line[1] << "\n";
	    if ( line_intersection(line,cluster._contour[j],cluster._contour[j+1]) )
	      break;
	  }

	//we didn't make it all the way around the contour, we intersected somewhere
	if ( j != cluster._contour.size()-1 )  break;

	//we did make it all the way around the contour, let check the last and first points
	if ( line_intersection(line, cluster._contour.back(), cluster._contour.front()) ) break;
      }
    
      // we broke, there was intersection, don't write this cluster
      if ( i != lines.size() ) continue;

      //it passed!
      oclusters.emplace_back(cluster);
      
    }
    

    /// Debug mode is off, return the result
    if ( ! meta.debug() ) return oclusters;
    
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
    
    return oclusters;
  }


  //It is based on an algorithm in  "Tricks of the Windows Game Programming Gurus" by Andre LeMothe
  bool HoughPTrackShower::line_intersection(const ::cv::Vec4i& hline,
					      const ::cv::Point& p1,
					      const ::cv::Point& p2) {
    
    auto& p0_x = hline[0];
    auto& p0_y = hline[1];

    auto& p1_x = hline[2];
    auto& p1_y = hline[3];

    auto& p2_x = p1.x;
    auto& p2_y = p1.y;

    auto& p3_x = p2.x;
    auto& p3_y = p2.y;
		
      
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) return true;
    
    return false;
    
  }
  
}
#endif
