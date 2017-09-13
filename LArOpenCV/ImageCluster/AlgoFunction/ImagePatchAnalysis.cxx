#ifndef __IMAGEPATCHANALYSIS_CXX_
#define __IMAGEPATCHANALYSIS_CXX_

#include "ImagePatchAnalysis.h"
#include "Geo2D/Core/Geo2D.h"
#ifndef __CLING__
#ifndef __CINT__
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#endif
#endif
#include "LArOpenCV/Core/laropencv_logger.h"
#include "LArOpenCV/Core/larbys.h"
#include "Contour2DAnalysis.h"
#include "SpectrumAnalysis.h"
#include <set>
#include <cmath>
#include <array>
#include <cassert>

namespace larocv {

  cv::Mat
  Flip(const cv::Mat& img, int flipCode) {
    cv::Mat dst_img(img.size(),img.type(),cv::Scalar(0));
    cv::flip(img,dst_img,flipCode);
    return dst_img;
  }
  
  cv::Mat
  Transpose(const cv::Mat& img) {
    cv::Mat dst_img(img.size(),img.type(),cv::Scalar(0));
    cv::transpose(img,dst_img);
    return dst_img;
  }
  
  cv::Mat
  BlankImage(const cv::Mat& img,uint val){
    cv::Mat dst_img(img.size(),img.type(),cv::Scalar(0));    
    dst_img.setTo(val);
    return dst_img;
  }

  cv::Mat
  LinearPolar(const cv::Mat& img,
	      geo2d::Vector<float> pt,
	      float radius) {
    cv::Mat res;
    cv::linearPolar(img, res, pt, radius, ::cv::WARP_FILL_OUTLIERS);
    return res;
  }
  
  GEO2D_Contour_t
  FindNonZero(const cv::Mat& img) {
    GEO2D_Contour_t pts_v;
    cv::findNonZero(img,pts_v);
    return pts_v;
  }

  size_t
  CountNonZero(const cv::Mat& img){
    return (size_t)cv::countNonZero(img);
  }


  float
  SumNonZero(const cv::Mat& img) {
    auto pts_v = FindNonZero(img);
    float sum = 0.0;
    for(const auto& pt : pts_v) {
      sum += (float)( (uchar)img.at<uchar>(pt.y,pt.x) );
    }
    return sum;
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
    auto polarimg = LinearPolar(img,circle.center,circle.radius*2);
    
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
    auto polarimg = LinearPolar(img, center, max_radi);

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
    CorrectEdgeRectangle(img,rect,range_x*2+1,range_y*2+1);
    
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
    if ( rect.x > img.cols - w ) rect.x = img.cols - w;
    if ( rect.y > img.rows - h ) rect.y = img.rows - h;

    if ( rect.x < 0 ) rect.x = 0;
    if ( rect.y < 0 ) rect.y = 0;

    if ( rect.width  >= img.cols ) rect.width  = img.cols - 1;
    if ( rect.height >= img.rows ) rect.height = img.rows - 1;
  }
  
  cv::Mat
  MaskImage(const cv::Mat& img,
	    const cv::Rect& rec,
	    int tol, bool maskout) {
    
    cv::Mat dst_img(img.size(),img.type(), cv::Scalar(0));
    cv::Mat mask(img.size(),img.type(), cv::Scalar(0));
    LAROCV_SDEBUG() << "Creating a mask for image (rows,cols) = (" << img.rows << "," << img.cols << ")"
		    << " with are rect mask @ (" << rec.x << "," << rec.y << ") "
		    << "w/ width " << rec.width << " height " << rec.height << std::endl;

    LAROCV_SWARNING() << "tol argument unused" << std::endl;
    
    mask(rec).setTo(cv::Scalar::all(255));

    if(maskout) cv::bitwise_not(mask,mask);
    
    img.copyTo(dst_img,mask);
    return dst_img;
  }

  bool
  PathExists(const cv::Mat& img,
	     const geo2d::Vector<float>& pt1,
	     const geo2d::Vector<float>& pt2,
	     float dthresh,
	     float pithresh,
	     uint  min_ctor_size) {
    
    bool DEBUG = false;
    if (DEBUG) {
      static int num1=0;
      std::stringstream ss;
      ss << "img_"<<num1<<"_0_p1_"<<pt1.x<<"_"<<pt1.y<<"_p2_"<<pt2.x<<"_"<<pt2.y<<".png";
      cv::imwrite(ss.str(),Threshold(img,10,255));
      num1+=1;
      std::cout << "Wrote: " << ss.str() << std::endl;
    }
    
    GEO2D_ContourArray_t ctor_v;
    if (pithresh>0.0)
      ctor_v = FindContours(Threshold(img,pithresh,255),min_ctor_size);
    else
      ctor_v = FindContours(img);

    if (DEBUG) {
      std::stringstream ss;
      static int num2=0;
      auto img_copy = BlankImage(img,0);
      cv::Scalar color(255);
      cv::drawContours(img_copy,ctor_v,-1,color);
      ss.str("");
      ss << "img_"<<num2<<"_1_p1_"<<pt1.x<<"_"<<pt1.y<<"_p2_"<<pt2.x<<"_"<<pt2.y<<".png";
      cv::imwrite(ss.str(),img_copy);
      num2+=1;
      std::cout << "Wrote: " << ss.str() << std::endl;
    }
    
    LAROCV_SDEBUG()<<"p1 ("<<pt1.x<<","<<pt1.y<<") & p2("<<pt2.x<<","<<pt2.y<<")"<<std::endl;
    
    double d1,d2;
    d1=d2=kINVALID_DOUBLE;
    auto id1 = FindContainingContour(ctor_v,pt1,d1);
    auto id2 = FindContainingContour(ctor_v,pt2,d2);
    
    if(id1==kINVALID_SIZE or d1==kINVALID_DOUBLE) {
      LAROCV_SDEBUG() << "Point 1 unassociated to contours in image!" << std::endl;
      return false;
    }
    
    if(id2==kINVALID_SIZE or d2==kINVALID_DOUBLE) {
      LAROCV_SDEBUG() << "Point 2 unassociated to contours in image!" << std::endl;
      return false;
    }    

    LAROCV_SDEBUG() << Pt2PtDistance(ctor_v[id1],ctor_v[id2]) << std::endl;
    LAROCV_SDEBUG() << "d1... " << d1 << " d2... " << d2 << std::endl;
    d1=std::abs(d1);
    if (d1 > dthresh) {
      LAROCV_SDEBUG() << "Point1 too far from any contour" << std::endl;
      return false;
    }

    d2=std::abs(d2);
    if (d2 > dthresh) {
      LAROCV_SDEBUG() << "Point2 too far from any contour" << std::endl;
      return false;
    }

    if (id1!=id2) {
      LAROCV_SDEBUG() << "Gap exists between two input points" << std::endl;
      return false;
    }
    
    return true;
  }

  float
  CircleDensity(const cv::Mat& img,
		const geo2d::Circle<float>& circle) {
    auto white_img = BlankImage(img,255);
    return CircleDensity(img,white_img,circle);
  }
  
  float
  CircleDensity(const cv::Mat& img,
		const cv::Mat& white_img,
		const geo2d::Circle<float>& circle) {
    float res = 0.0;
    auto sze = CountNonZero(white_img,circle,0);
    auto npx = CountNonZero(img      ,circle,0);
    if (npx) res = (float) npx / (float) sze;
    return res;
  }
  
  bool
  ChargeBlobCircleEstimate(const cv::Mat& img,
			   const geo2d::Vector<float>& center,
			   geo2d::Circle<float>& res,
			   float thresh,
			   float start_rad,
			   float end_rad,
			   float step) {

    res.center=geo2d::Vector<float>(kINVALID_FLOAT,kINVALID_FLOAT);
    res.radius=kINVALID_FLOAT;
    
    auto npx_start = CountNonZero(img,geo2d::Circle<float>(center,start_rad),0);
    if (!npx_start) {
      LAROCV_SDEBUG() << "Found no charge blob here @ " << center << " rad " << start_rad << std::endl;
      return false;
    }
    
    std::vector<float> rad_v,cratio_v;
    
    auto white_img = BlankImage(img,255);
    
    for(auto this_rad=start_rad; this_rad <= end_rad; this_rad+=step) {

      geo2d::Circle<float> this_circle(center,this_rad);
      
      auto cratio = CircleDensity(img,white_img,this_circle);

      rad_v.push_back(this_rad);
      cratio_v.push_back(cratio);
    }

    
    for(size_t radid=0;radid<rad_v.size();++radid) {
      auto cratio = cratio_v[radid];
      
      LAROCV_SDEBUG() << radid << ") @ center " << center << " rad " << rad_v[radid]
		      << " den " << cratio << std::endl;

      if (cratio>thresh) continue;
      
      res.center = center;
      res.radius = rad_v[radid];
      
      return true;
    }

    res.center=geo2d::Vector<float>(kINVALID_FLOAT,kINVALID_FLOAT);
    res.radius=kINVALID_FLOAT;
    return false;
  }

  geo2d::VectorArray<float>
  QPointOnCircleRefine(const cv::Mat& img,
		       const geo2d::Circle<float>& circle,
		       const geo2d::VectorArray<float>& xs_v,
		       const float mask_inner) {

    geo2d::VectorArray<float> res_v;
    res_v.reserve(xs_v.size());
    
    auto mask_img = MaskImage(img,circle,0,false);
    mask_img = MaskImage(mask_img,geo2d::Circle<float>(circle.center,mask_inner),0,true);
    auto ctor_v = FindContours(mask_img);

    std::vector<geo2d::VectorArray<float> > xs_ctor_vv;
    xs_ctor_vv.resize(ctor_v.size());
    for(auto& xs_ctor_v : xs_ctor_vv) xs_ctor_v.reserve(xs_v.size());
	  
    for(const auto& xs : xs_v) {
      auto id = FindContainingContour(ctor_v,xs);
      if (id==kINVALID_SIZE)
	res_v.push_back(xs);
      else
	xs_ctor_vv.at(id).push_back(xs);
    }

    for(const auto& xs_ctor_v : xs_ctor_vv) {
      if (xs_ctor_v.empty())
	continue;
      else if (xs_ctor_v.size()==1)
	res_v.push_back(xs_ctor_v.front());
      else {
	auto xs = geo2d::AngularAverage(circle,xs_ctor_v);
	res_v.push_back(xs);
      }
    }
    
    return res_v;
  }

  geo2d::VectorArray<float>
  QPointOnCircleRefine(const cv::Mat& img,
		       const geo2d::Circle<float>& circle,
		       const float mask_inner,
		       const float pi_threshold,
		       const float supression) {
    
    auto xs_v = QPointOnCircle(img,circle,pi_threshold,supression);
    return QPointOnCircleRefine(img,circle,xs_v,mask_inner);
  }

  bool
  Contained(const cv::Mat& img,
	    const geo2d::Vector<float>& pt) {

    if (pt.x >= img.cols) return false;
    if (pt.y >= img.rows) return false;
    if (pt.x < 0) return false;
    if (pt.y < 0) return false;

    return true;
  }

  geo2d::Vector<float>
  EstimateMidPoint(const cv::Mat& img,
		   const geo2d::Vector<float>& pt) {
    
    geo2d::Vector<float> res;

    std::array<geo2d::VectorArray<float>,4> cross_vv;
    std::array<geo2d::Vector<float>,4> dir_v;
    dir_v[0] = geo2d::Vector<float>( 1,  0);
    dir_v[1] = geo2d::Vector<float>( 0,  1);
    dir_v[2] = geo2d::Vector<float>(-1,  0);
    dir_v[3] = geo2d::Vector<float>( 0, -1);
      
    for(size_t dir_id=0; dir_id<4; ++dir_id) {
      auto& cross_v = cross_vv[dir_id];
	
      auto dir = dir_v[dir_id];
      auto dir_pt = pt;
      int dir_ctr = 0;
      
      uint px = kINVALID_UINT;

      while(px and Contained(img,dir_pt)) {
	cross_v.push_back(dir_pt);
	px = (uint) img.at<uchar>(dir_pt.y,dir_pt.x);
	dir_ctr += 1;
	dir_pt = pt + dir_ctr * dir;
      }
    }
    
    size_t min_dir_id = kINVALID_SIZE;
    size_t min_size = kINVALID_SIZE;
    for(size_t dir_id=0;dir_id<4;++dir_id) {
      const auto& cross_v = cross_vv[dir_id];
      if (cross_v.empty()) throw larbys("Cross should have initial pt");
      if (cross_v.size() == 1) continue;
      if (cross_v.size() < min_size) {
	min_size = cross_v.size();
	min_dir_id = dir_id;
      }
    }

    if (min_dir_id == kINVALID_SIZE)
      return pt;
      
    // get the mid point
    size_t mid_id = std::ceil((float) min_size / (float) 2.0);

    res = cross_vv.at(min_dir_id).at(mid_id);
      
    return res;
  }
  
}
#endif
