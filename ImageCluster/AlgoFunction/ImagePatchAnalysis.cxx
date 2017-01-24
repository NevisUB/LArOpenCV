#ifndef __IMAGEPATCHANALYSIS_CXX_
#define __IMAGEPATCHANALYSIS_CXX_

#include "ImagePatchAnalysis.h"
#include "Core/Geo2D.h"
#include "opencv2/imgproc.hpp"
#include "Core/laropencv_logger.h"
#include "Core/larbys.h"

namespace larocv {
  
  geo2d::VectorArray<float>
  QPointOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle, const float pi_threshold)
  {
    geo2d::VectorArray<float> res;
    
    // Find crossing point
    ::cv::Mat polarimg;
    ::cv::linearPolar(img, polarimg, circle.center, circle.radius*2, ::cv::WARP_FILL_OUTLIERS);
    
    size_t col = (size_t)(polarimg.cols / 2);
    
    std::vector<std::pair<int,int> > range_v;
    std::pair<int,int> range(-1,-1);
    
    for(size_t row=0; row<polarimg.rows; ++row) {
      
      float q = (float)(polarimg.at<unsigned char>(row, col));
      if(q < pi_threshold) {
	if(range.first >= 0) {
	  range_v.push_back(range);
	  range.first = range.second = -1;
	}
	continue;
      }
      //std::cout << row << " / " << polarimg.rows << " ... " << q << std::endl;
      if(range.first < 0) range.first = range.second = row;
      
      else range.second = row;
      
    }
    if(range.first>=0 && range.second>=0) range_v.push_back(range);
    // Check if end should be combined w/ start
    if(range_v.size() >= 2) {
      if(range_v[0].first == 0 && (range_v.back().second+1) == polarimg.rows) {
	range_v[0].first = range_v.back().first - (int)(polarimg.rows);
	range_v.pop_back();
      }
    }
    // Compute xs points
    for(auto const& r : range_v) {
      
      //std::cout << "XS @ " << r.first << " => " << r.second << " ... " << polarimg.rows << std::endl;
      float angle = ((float)(r.first + r.second))/2.;
      if(angle < 0) angle += (float)(polarimg.rows);
      angle = angle * M_PI * 2. / ((float)(polarimg.rows));
      
      geo2d::Vector<float> pt;
      pt.x = circle.center.x + circle.radius * cos(angle);
      pt.y = circle.center.y + circle.radius * sin(angle);
      
      res.push_back(pt);
    }
    return res;
  }

  geo2d::Vector<float> MeanPixel(const cv::Mat& img, const geo2d::Vector<float>& center,
				 size_t range_x, size_t range_y, float pi_threshold)
  {
    // Make a better guess
    ::cv::Rect rect(center.x - range_x, center.y - range_y, range_x*2+1,range_y*2+1);
    ::larocv::CorrectEdgeRectangle(img,rect,range_x*2+1,range_y*2+1);
    
    LAROCV_SDEBUG() << "rows cols " << img.rows
		    << "," << img.cols << " and rect " << rect << std::endl;
    auto small_img = ::cv::Mat(img,rect);
    ::cv::Mat thresh_small_img;
    ::cv::threshold(small_img,thresh_small_img,pi_threshold,1,CV_THRESH_BINARY);
    
    geo2d::VectorArray<int> points;
    ::cv::findNonZero(thresh_small_img, points);
    
    geo2d::Vector<float> trial_pt(-1,-1);
    if(points.empty()) return trial_pt;
    
    for (unsigned i = 0; i < points.size(); ++i){
      trial_pt.x += points[i].x;
      trial_pt.y += points[i].y;
    }
    
    trial_pt.x /= (float)(points.size());
    trial_pt.y /= (float)(points.size());
    
    trial_pt.x = center.x + trial_pt.x - range_x;
    trial_pt.y = center.y + trial_pt.y - range_y;

    return trial_pt;
  }
  
  void FindEdges(const GEO2D_Contour_t& ctor,
		 geo2d::Vector<float>& edge1,
		 geo2d::Vector<float>& edge2)
  {
    // cheap trick assuming this is a linear, linear track cluster
    geo2d::Vector<float> mean_pt, ctor_pt;
    mean_pt.x = mean_pt.y = 0.;
    for(auto const& pt : ctor) { mean_pt.x += pt.x; mean_pt.y += pt.y; }
    mean_pt.x /= (double)(ctor.size());
    mean_pt.y /= (double)(ctor.size());
    // find the furthest point from the mean (x,y)
    double dist_max=0;
    double dist;
    for(auto const& pt : ctor) {
      ctor_pt.x = pt.x;
      ctor_pt.y = pt.y;
      dist = geo2d::dist(mean_pt,ctor_pt);
      if(dist > dist_max) {
	edge1 = pt;
	dist_max = dist;
      }
    }
    // find the furthest point from edge1
    dist_max=0;
    for(auto const& pt : ctor) {
      ctor_pt.x = pt.x;
      ctor_pt.y = pt.y;
      dist = geo2d::dist(edge1,ctor_pt);
      if(dist > dist_max) {
	edge2 = pt;
	dist_max = dist;
      }
    }
  }

  geo2d::Line<float> SquarePCA(const ::cv::Mat& img,
			       geo2d::Vector<float> pt,
			       float width, float height)
  {

    cv::Rect rect(pt.x-width, pt.y-height, 2*width+1, 2*height+1);
    ::larocv::CorrectEdgeRectangle(img,rect,2*width+1,2*height+1);
    
    auto small_img = ::cv::Mat(img,rect);
    ::cv::Mat thresh_small_img;
    //::cv::threshold(small_img,thresh_small_img,_pi_threshold,1,CV_THRESH_BINARY);
    ::cv::threshold(small_img,thresh_small_img,1,1000,3);
    geo2d::VectorArray<int> points;
    findNonZero(thresh_small_img, points);

    if(points.size() < 2) {
      LAROCV_SDEBUG() << "SquarePCA approx cannot be made (# points " << points.size() << " < 2)" << std::endl;
      throw larbys("SquarePCA found no point...");
    }
    
    cv::Mat mat_points(points.size(), 2, CV_32FC1);
    for (unsigned i = 0; i < mat_points.rows; ++i) {
      mat_points.at<float>(i, 0) = points[i].x;
      mat_points.at<float>(i, 1) = points[i].y;
    }

    ::cv::PCA pcaobj(mat_points,::cv::Mat(),::cv::PCA::DATA_AS_ROW,0);
    
    pt.x += pcaobj.mean.at<float>(0,0) - width;
    pt.y += pcaobj.mean.at<float>(0,1) - height;
    
    auto dir = geo2d::Vector<float>(pcaobj.eigenvectors.at<float>(0,0),
				    pcaobj.eigenvectors.at<float>(0,1));

    //std::cout << "Mean @ (" << pt.x << "," << pt.y << ") ... dir (" << dir.x << "," << dir.y << std::endl;

    return geo2d::Line<float>(pt,dir);
  }

  void CorrectEdgeRectangle(const ::cv::Mat& img, cv::Rect& rect,int w, int h)
  {

    //make it edge aware
    if ( rect.x < 0 ) rect.x = 0;
    if ( rect.y < 0 ) rect.y = 0;
    
    if ( rect.x > img.cols - w ) rect.x = img.cols - w;
    if ( rect.y > img.rows - h ) rect.y = img.rows - h;
    
  }
  


  
}
#endif
