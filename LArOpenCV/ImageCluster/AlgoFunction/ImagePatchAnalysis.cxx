#ifndef __IMAGEPATCHANALYSIS_CXX_
#define __IMAGEPATCHANALYSIS_CXX_

#include "ImagePatchAnalysis.h"
#include "Geo2D/Core/Geo2D.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "LArOpenCV/Core/laropencv_logger.h"
#include "LArOpenCV/Core/larbys.h"
#include "Contour2DAnalysis.h"
#include "SpectrumAnalysis.h"
#include <set>

namespace larocv {

  GEO2D_Contour_t
  FindNonZero(const cv::Mat& img) {
    GEO2D_Contour_t pts_v;
    cv::findNonZero(img,pts_v);
    return pts_v;
  }
  
  double
  MeanDistanceToLine(const cv::Mat& img,
		     const geo2d::Line<float>& line) {
    GEO2D_Contour_t pts_v;
    std::vector<float> dist_v;
    cv::findNonZero(img,pts_v);
    if (pts_v.empty()) throw larbys("No points found");
    dist_v.reserve(pts_v.size());
    for(const auto& pt : pts_v) {
      geo2d::Vector<float> pt_f(pt);
      dist_v.emplace_back(geo2d::Distance(line,pt_f));
    }
    return Mean(dist_v);
  }
  
  double
  SigmaDistanceToLine(const cv::Mat& img,
		      const geo2d::Line<float>& line) {
    GEO2D_Contour_t pts_v;
    std::vector<float> dist_v;
    cv::findNonZero(img,pts_v);
    if (pts_v.empty()) throw larbys("No points found");
    dist_v.reserve(pts_v.size());
    for(const auto& pt : pts_v) {
      geo2d::Vector<float> pt_f(pt);
      dist_v.emplace_back(geo2d::Distance(line,pt_f));
    }
    return Sigma(dist_v);
  }
  
  cv::Mat
  Threshold(const cv::Mat& mat,
	    double thresh,
	    double max_val) {
    auto mat_copy = mat.clone();
    cv::threshold(mat,mat_copy,thresh,max_val,0);
    return mat_copy;
  }
  
  geo2d::VectorArray<float>
  QPointOnCircle(const ::cv::Mat& img,
		 const geo2d::Circle<float>& circle,
		 const float pi_threshold,
		 const float supression)
  {
    cv::Mat polarimg;
    cv::linearPolar(img, polarimg, circle.center, circle.radius*2, ::cv::WARP_FILL_OUTLIERS);
    
    size_t col = (size_t)(polarimg.cols / 2);
    return RadialIntersections(polarimg,circle,col,pi_threshold,supression);
  }

  std::vector<geo2d::VectorArray<float> >
  QPointArrayOnCircleArray(const ::cv::Mat& img,
			   const geo2d::Vector<float>& center,
			   const std::vector<float>& radius_v,
			   const float pi_threshold,
			   const float supression)
  {
    LAROCV_SDEBUG() << " called with center " << center
		    << ", " << radius_v.size()
		    << " radii, px thresh " << pi_threshold
		    << ", & sup " << supression << std::endl;
    
    std::vector<geo2d::VectorArray<float> > res_v;
    if(radius_v.empty()) return res_v;
    float max_radi = 0;
    float min_radi = -1;
    for(auto const& r : radius_v) {
      if(r<=0) throw larbys("non-positive radius cannot be processed!");
      if(min_radi<0) { min_radi = max_radi = r; continue; }
      if(max_radi < r) max_radi = r;
      if(min_radi > r) min_radi = r;
    }
    if(max_radi == min_radi) max_radi *= 1.1;
    else max_radi += (max_radi - min_radi);
    
    // Find crossing point
    cv::Mat polarimg;
    cv::linearPolar(img, polarimg, center, max_radi, ::cv::WARP_FILL_OUTLIERS);

    std::vector<size_t> col_v(radius_v.size(),0);
    for(size_t r_idx=0; r_idx<radius_v.size(); ++r_idx) {
      auto const& radius = radius_v[r_idx];
      col_v[r_idx] = (size_t)(radius / max_radi * (float)(polarimg.cols) + 0.5);
    }

    for(size_t col_idx=0; col_idx<col_v.size(); ++col_idx) {

      auto const& col    = col_v[col_idx];
      auto const& radius = radius_v[col_idx];
      LAROCV_SDEBUG() << "... @ radius " << radius << std::endl;
	
      auto res = RadialIntersections(polarimg,geo2d::Circle<float>(center,radius),col,pi_threshold,supression);
	
      res_v.emplace_back(std::move(res));
    }
    return res_v;
  }

  
  geo2d::VectorArray<float>
  RadialIntersections(const ::cv::Mat& polarimg,
		      const geo2d::Circle<float>& circle,
		      const size_t col,
		      const float pi_threshold,
		      const float supression)
  {

    if (col > polarimg.cols) {
      LAROCV_SCRITICAL() << "Requested column " << col
			 << " outside max column " << polarimg.cols << std::endl;
      throw larbys();
    }
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

    std::vector<std::pair<float,float> > range_a_w_v;
    range_a_w_v.reserve(range_a_w_v.size());
    
    for(auto const& r : range_v) {
      float angle = ((float)(r.first + r.second))/2.;
      if(angle < 0) angle += (float)(polarimg.rows);
      angle = angle * M_PI * 2. / ((float)(polarimg.rows));
      float width = std::abs(r.second - r.first);
      range_a_w_v.emplace_back(angle,width);
    }

    auto npts = range_a_w_v.size();
    
    if(supression>0 and npts>1) {
      LAROCV_SDEBUG() << "Applying angular supression" << std::endl;
      
      std::vector<std::pair<float,float> > tmp_v;
      tmp_v.reserve(range_a_w_v.size());

      if (npts==2) {
	if (std::abs(range_a_w_v[0].first - range_a_w_v[1].first) < supression) {
	  tmp_v.emplace_back(range_a_w_v[0].second > range_a_w_v[1].second ? range_a_w_v[0] : range_a_w_v[1]);
	  std::swap(tmp_v,range_a_w_v);
	}
      }
      else if (npts>2) {
	std::set<size_t> preserved_s;
	std::set<size_t> modified_s;
	
	LAROCV_SDEBUG() << "Applying angular supression" << std::endl;
	for(size_t r1=0;r1<npts;r1++) { 
	  for(size_t r2=r1+1;r2<npts;r2++) {
	    if (std::abs(range_a_w_v[r1].first-range_a_w_v[r2].first) < supression) {
	      size_t pidx = range_a_w_v[r1].second > range_a_w_v[r2].second ? r1 : r2;
	      size_t ridx = pidx == r1 ? r2 : r1;
	      preserved_s.insert(pidx);
	      modified_s.insert(pidx);
	      modified_s.insert(ridx);
	    }
	  }
	}
	for(auto pidx : preserved_s) tmp_v.emplace_back(range_a_w_v[pidx]);
	for(size_t r1=0;r1<npts;r1++) {
	  if (modified_s.find(r1) != modified_s.end()) continue;
	  tmp_v.emplace_back(range_a_w_v[r1]);
	}
	LAROCV_SDEBUG() << "Supressed " << npts << " to " << tmp_v.size() << std::endl; 
	std::swap(tmp_v,range_a_w_v);
      }
      
    }
    // Compute xs points
    geo2d::VectorArray<float> res;
    res.reserve(range_a_w_v.size());
    for(auto const& aw : range_a_w_v) {
      geo2d::Vector<float> pt;
      pt.x = circle.center.x + circle.radius * cos(aw.first);
      pt.y = circle.center.y + circle.radius * sin(aw.first);
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
    LAROCV_SDEBUG() << "SquarePCA @ pt " << pt << " width " << width << " height " << height << std::endl;
    cv::Rect rect(pt.x-width, pt.y-height, 2*width+1, 2*height+1);
    LAROCV_SDEBUG() << "set rect @ " << rect << std::endl;
    CorrectEdgeRectangle(img,rect,2*width+1,2*height+1);
    LAROCV_SDEBUG() << "corrected rect @ " << rect << std::endl;
	
    auto small_img = cv::Mat(img,rect);
    cv::Mat thresh_small_img;
    cv::threshold(small_img,thresh_small_img,1,255,0);
    
    auto pca = CalcPCA(small_img);
    
    pt.x += pca.pt.x - width;
    pt.y += pca.pt.y - height;

    pca.pt = pt;
    
    return pca;
  }

  double
  SquareR(const ::cv::Mat& img,
	  geo2d::Vector<float> pt,
	  float width, float height) {
    
    LAROCV_SDEBUG() << "SquareR @ pt " << pt << " width " << width << " height " << height << std::endl;
    cv::Rect rect(pt.x-width, pt.y-height, 2*width+1, 2*height+1);
    LAROCV_SDEBUG() << "set rect @ " << rect << std::endl;
    CorrectEdgeRectangle(img,rect,2*width+1,2*height+1);
    LAROCV_SDEBUG() << "corrected rect @ " << rect << std::endl;

    auto small_img = cv::Mat(img,rect);
    
    GEO2D_Contour_t nonzero;
    cv::findNonZero(small_img, nonzero);

    //lets keep the same type as pixel location
    std::vector<float> x_v;
    x_v.reserve(nonzero.size());
    std::vector<float> y_v;
    y_v.reserve(nonzero.size());
    
    for(const auto& pt : nonzero) {
      x_v.push_back(pt.x);
      y_v.push_back(pt.y);
    }

    double numerator   = Covariance(x_v,y_v);
    double denominator = Sigma(x_v) * Sigma(y_v);

    
    double r = denominator > 0 ? numerator / denominator : 0.0;
    
    return r;
  }
  
  
  void
  CorrectEdgeRectangle(const ::cv::Mat& img,
		       cv::Rect& rect,
		       int w, int h)
  {

    //make it edge aware
    if ( rect.x < 0 ) rect.x = 0;
    if ( rect.y < 0 ) rect.y = 0;
    
    if ( rect.x > img.cols - w ) rect.x = img.cols - w;
    if ( rect.y > img.rows - h ) rect.y = img.rows - h;
    
  }
  
  cv::Mat
  MaskImage(const cv::Mat& img,
	    const cv::Rect& rec,
	    int tol, bool maskout) {
    
    cv::Mat dst_img(img.size(),img.type(), CV_8UC1);
    cv::Mat mask = cv::Mat(img.size(),img.type(),CV_8UC1);
    LAROCV_SDEBUG() << "Creating a mask for image (rows,cols) = (" << img.rows << "," << img.cols << ")"
		    << " with are rect mask @ (" << rec.x << "," << rec.y << ") "
		    << "w/ width " << rec.width << " height " << rec.height << std::endl;

    LAROCV_SWARNING() << "tol argument unused" << std::endl;
    
    mask(rec).setTo(cv::Scalar::all(255));

    if(maskout) cv::bitwise_not(mask,mask);
    
    img.copyTo(dst_img,mask);
    return dst_img;
  }


}
#endif
