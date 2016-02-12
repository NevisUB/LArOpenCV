//by vic
#ifndef __IMAGECLUSTER_PCAUTILITIES_H__
#define __IMAGECLUSTER_PCAUTILITIES_H__

#include <vector>
#include <cmath>
#include <utility>

#include "PCABox.h"
//maybe redundant
#include <opencv2/opencv.hpp>

namespace larcv {
    
  double mean ( ::std::vector<int>& data,
		size_t start, size_t end );
  
  double stdev( ::std::vector<int>& data,
		size_t start, size_t end );
  
  
  double cov  ( ::std::vector<int>& data1,
		::std::vector<int>& data2,
		size_t start, size_t end );

  void pca_line(Contour_t cluster_s,
		const ::cv::Rect& rect,
		std::vector<double>& line,
		Point2D& e_vec,
		Point2D& e_center);
  
  void pca_line(Contour_t cluster_s,
		const ::cv::Rect& roi,
		const ::cv::Rect& rect,
		std::vector<double>& line,
		Point2D& e_vec,
		Point2D& e_center);
  
  double get_roi_cov(const Contour_t & pts);

  std::pair<double,double> get_mean_loc(const ::cv::Rect& rect,
					const Contour_t&  pts );
  
  int get_charge_sum(const ::cv::Mat& subImg, const Contour_t& pts );

  std::pair<double,double> closest_point_on_line(std::vector<double>& line,int lx,int ly);

  double distance_to_line(std::vector<double>& line,int lx,int ly);

  Point2D intersection_point(const std::vector<double>& l1, const std::vector<int> l2);

  bool compatible (const PCABox& b1, const PCABox& b2);
  bool intersect  (const PCABox& b1, const PCABox& b2);
  bool check_angle(const PCABox& b1, const PCABox& b2);
  
  std::vector<PCABox> sub_divide(PCABox& box, short divisions);
  int decide_axis(std::vector<PCABox>& boxes, std::map<int,std::vector<int> > connections);
  
}
#endif
