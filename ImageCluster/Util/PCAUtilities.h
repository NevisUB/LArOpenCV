//by vic
#ifndef __IMAGECLUSTER_PCAUTILITIES_H__
#define __IMAGECLUSTER_PCAUTILITIES_H__

#include <vector>
#include <cmath>
#include <utility>

#include "PCAPath.h"
// #include "PCABox.h"
//maybe redundant
#include <opencv2/opencv.hpp>

namespace larocv {
  
  double roi_d_to_line(const std::vector<double>& line,int lx,int ly);  
  double roi_d_to_line(const Point2D& dir, const Point2D& pt,int lx,int ly);
  
  double total_d_pca   (const PCABox& box);
  double total_cw_d_pca(const PCABox& box);

  double avg_d_pca(const PCABox& box);
  double cw_d_pca (const PCABox& box);

  void pca_line(const Contour_t& cluster_s,
		Point2D& e_vec,
		Point2D& e_center);

  void pca_line(const Contour_t& cluster_s,
		Point2D& e_vec_first,
		Point2D& e_vec_second,
		double&  e_val_first,
		double&  e_val_second,
		Point2D& e_center);
  
  void pca_line(const Contour_t& cluster_s,
		const ::cv::Rect& rect,
		std::vector<double>& line,
		Point2D& e_vec,
		Point2D& e_center);
  
  void pca_line(const Contour_t& cluster_s,
		const ::cv::Rect& roi,
		const ::cv::Rect& rect,
		std::vector<double>& line,
		Point2D& e_vec,
		Point2D& e_center);
  
  void pca_line(const Contour_t& cluster_s,
		Point2D& e_vec_first,
		Point2D& e_vec_second,
		Point2D& e_center);
      
  
  
  std::pair<double,double> get_mean_loc(const ::cv::Rect& rect,
					const Contour_t&  pts );
  
  int get_charge_sum(const ::cv::Mat& subImg, const Contour_t& pts );

  inline double dist(const ::cv::Point2f& a, const ::cv::Point2f& b) { return std::sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)); }
  inline double dist(const Point2D& a, const ::cv::Point2f& b)       { return std::sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)); }
  inline double dist(const ::cv::Point2f& a,const Point2D& b)        { return std::sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)); }
  inline double dist(const ::cv::Point2d& a, const ::cv::Point2d& b) { return std::sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)); }
  inline double dist(const Point2D& a, const Point2D& b)             { return std::sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)); }
  inline double dist(double x1, double x2, double y1, double y2)     { return std::sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)); }

  
  std::pair<double,double> closest_point_on_line(std::vector<double>& line,int lx,int ly);

  Point2D intersection_point(const std::vector<double>& l1, const std::vector<int> l2);
  Point2D intersection_point(double x1, double x2, double y1, double y2,
			     double x3, double x4, double y3, double y4);
   
  bool compatible (const PCABox& b1, const PCABox& b2);
  bool intersect  (const PCABox& b1, const PCABox& b2);
  bool check_angle(const PCABox& b1, const PCABox& b2);
  
  std::vector<PCABox> sub_divide(PCABox& box, short divisions);

  PCAPath decide_axis(std::vector<PCABox>& boxes, std::map<int,std::vector<int> > connections);

  
}
#endif
