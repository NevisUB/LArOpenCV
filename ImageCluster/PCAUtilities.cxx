//by vic
#ifndef __IMAGECLUSTER_PCAUTILITIES_CXX__
#define __IMAGECLUSTER_PCAUTILITIES_CXX__

#include "PCAUtilities.h"

namespace larcv {

  double stdev( ::std::vector<int>& data,
		size_t start, size_t end )
  {

    if ( (end - start) <=0 ) return 0.0;
    
    double result = 0.0;
    auto    avg   = mean(data,start,end);
    
    for(int i = start; i <= end; ++i)
      result += (data.at(i) - avg)*(data.at(i) - avg);
    
    return ::std::sqrt(result/((double)(end - start)));
  }

  
  double cov ( ::std::vector<int>& data1,
	       ::std::vector<int>& data2,
	       size_t start, size_t end )
  {
    
    if ( start == end ) return 0.0;
    
    double result = 0.0;
    auto   mean1  = mean(data1,start,end);
    auto   mean2  = mean(data2,start,end);
    
    for(int i = start; i <= end; ++i)
      result += (data1.at(i) - mean1)*(data2.at(i) - mean2);
    
    return result/((double)(end - start));
  }
  
  double mean( ::std::vector<int>& data,
	       size_t start, size_t end )
  {
    if ( start == end ) return 0.0;
    double result = 0.0;
    
    for(int i = start; i <= end; ++i)
      result += data.at(i);
    
    return result / ((double)( end - start ));
  }  
  
  bool pca_line(Contour_t cluster_s,
		const ::cv::Rect& roi, // assumes this is ROI coming in...
		std::vector<double>& line,
		Point2D& e_vec,
		Point2D& e_center) {
    
    ::cv::Mat ctor_pts(cluster_s.size(), 2, CV_64FC1);
    
    for (unsigned i = 0; i < ctor_pts.rows; ++i) {
      ctor_pts.at<double>(i, 0) = cluster_s[i].x;
      ctor_pts.at<double>(i, 1) = cluster_s[i].y;
    }
    
    ::cv::PCA pca_ana(ctor_pts, ::cv::Mat(), CV_PCA_DATA_AS_ROW,0);

    e_center = Point2D( pca_ana.mean.at<double>(0,0),
			pca_ana.mean.at<double>(0,1) );
    
    e_vec    = Point2D( pca_ana.eigenvectors.at<double>(0,0),
			pca_ana.eigenvectors.at<double>(0,1) );
    
    auto& ax = e_vec.x;
    auto& ay = e_vec.y;
    auto  sq = std::sqrt(ax*ax + ay*ay);
    
    ax /= sq;
    ay /= sq;
    
    line[0] = 0 + roi.x;
    line[1] = e_center.y + ( (0 - e_center.x) / e_vec.x ) * e_vec.y + roi.y;
    line[2] = roi.width + roi.x;
    line[3] = e_center.y + ( (roi.width - e_center.x) / e_vec.x) * e_vec.y + roi.y;
    
    return true;
  }
  
  bool pca_line(Contour_t cluster_s,
		const ::cv::Rect& roi,
		const ::cv::Rect& rect,
		std::vector<double>& line,
		Point2D& e_vec,
		Point2D& e_center) {
      
    ::cv::Mat ctor_pts(cluster_s.size(), 2, CV_64FC1);
      
    for (unsigned i = 0; i < ctor_pts.rows; ++i) {
      ctor_pts.at<double>(i, 0) = cluster_s[i].x;
      ctor_pts.at<double>(i, 1) = cluster_s[i].y;
    }
    
    ::cv::PCA pca_ana(ctor_pts, ::cv::Mat(), CV_PCA_DATA_AS_ROW,0);

    e_center = Point2D( pca_ana.mean.at<double>(0,0),
			pca_ana.mean.at<double>(0,1) );
    
    e_vec    = Point2D( pca_ana.eigenvectors.at<double>(0,0),
			pca_ana.eigenvectors.at<double>(0,1) );
    
    auto& ax = e_vec.x;
    auto& ay = e_vec.y;

    auto cx = e_center.x;
    auto cy = e_center.y;

    auto  sq = std::sqrt(ax*ax + ay*ay);
    
    ax /= sq;
    ay /= sq;
    
    line[0] = rect.x;
    line[1] = cy + ( (rect.x - cx) / e_vec.x ) * e_vec.y ;
    line[2] = rect.x + rect.width;
    line[3] = cy + ( (rect.x + rect.width - cx) / e_vec.x ) * e_vec.y;
    
    line = {line[0]+roi.x,line[1]+roi.y,line[2]+roi.x,line[3]+roi.y};
		     
    return true;
  }

  
  double get_roi_cov(const Contour_t & pts) {

    std::vector<int> x_; x_.resize(pts.size()); 
    std::vector<int> y_; y_.resize(pts.size());
    for(unsigned i = 0; i < pts.size(); ++i)
      { x_[i] = pts[i].x; y_[i] = pts[i].y; }
    
    double pearsons_r;
    auto co = cov  (x_,y_,0,pts.size() - 1);
    auto sx = stdev(x_,   0,pts.size() - 1);
    auto sy = stdev(y_,   0,pts.size() - 1);

    if ( sx != 0 && sy != 0) // trunk exists
      pearsons_r =  co / ( sx * sy );
    else
      pearsons_r = 0;
    
    return pearsons_r;
  }


  double distance_to_line(std::vector<double>& line,int lx,int ly) {

    //real time collision detection page 128
    //closest point on line
    auto ax = line[0];
    auto ay = line[1];
    auto bx = line[2];
    auto by = line[3];
	
    auto abx = bx-ax;
    auto aby = by-ay;
	
    auto t = ( (lx-ax)*abx + (ly-ay)*aby ) / (abx*abx + aby*aby);
    auto dx = ax + t * abx;
    auto dy = ay + t * aby;
    auto dist = (dx-lx)*(dx-lx) + (dy-ly)*(dy-ly); //squared distance
    return dist;
  }
  
  std::pair<double,double> closest_point_on_line(std::vector<double>& line,int lx,int ly) {
    
    //real time collision detection page 128
    //closest point on line
    auto ax = line[0];
    auto ay = line[1];
    auto bx = line[2];
    auto by = line[3];
	
    auto abx = bx-ax;
    auto aby = by-ay;
	
    auto t = ( (lx-ax)*abx + (ly-ay)*aby ) / (abx*abx + aby*aby);
    auto dx = ax + t * abx;
    auto dy = ay + t * aby;

    return std::make_pair(dx,dy);
  }


  std::pair<double,double> get_mean_loc(const ::cv::Rect& rect,
					const Contour_t&  pts ) {
    
    double mean_x = 0.0;
    double mean_y = 0.0;
    
    for(const auto& pt : pts) { mean_x += pt.x;  mean_y += pt.y; }
    
    return { mean_x / ( (double) pts.size() ) + rect.x,mean_y / ( (double) pts.size() ) + rect.y};
  }
  
  int get_charge_sum(const ::cv::Mat& subImg, const Contour_t& pts ) {
    int charge_sum = 0;
    for(const auto& pt : pts ) charge_sum += (int) subImg.at<uchar>(pt.y,pt.x);
    return charge_sum;
  }

  Point2D intersection_point(const std::vector<double>& l1, const std::vector<int> l2) {
    
    Point2D pt;

    auto& x1 = l1[0]; double x3 = l2[0]; 
    auto& x2 = l1[2]; double x4 = l2[2];
    auto& y1 = l1[1]; double y3 = l2[1];
    auto& y2 = l1[3]; double y4 = l2[3];
    
    auto denom = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    pt.x = ( (x1*y2 - y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4) ) / denom;
    pt.y = ( (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4) ) / denom;
    
    return pt;
    
  }
  
}

#endif
