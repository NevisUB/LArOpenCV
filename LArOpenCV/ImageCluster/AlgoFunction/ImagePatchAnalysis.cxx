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
#define T2R 0.0175
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
    cv::linearPolar(img, res, pt, radius, cv::WARP_FILL_OUTLIERS);
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
  QPointOnCircle(const cv::Mat& img,
		 const geo2d::Circle<float>& circle,
		 const float pi_threshold,
		 const float asup,
		 const float wsup)
  {
    bool inside = false;
    auto polarimg = LinearPolar(img,circle.center,circle.radius*2);
    size_t col = (size_t)(polarimg.cols / 2);

    auto ret_v = RadialIntersections(polarimg,circle,col,pi_threshold,asup,wsup);

    geo2d::VectorArray<float> tmp_v;
    tmp_v.reserve(ret_v.size());
    for(auto& ret : ret_v) {
      inside  = true;
      inside &= ((ret.y>0) and (ret.x>0));
      inside &= ((ret.y<img.rows) and (ret.x<img.cols));
      if (inside and img.at<uchar>(ret.y,ret.x)) 
	tmp_v.emplace_back(std::move(ret));
    }
    return tmp_v;
  }

  

  std::vector<geo2d::VectorArray<float> >
  QPointArrayOnCircleArray(const cv::Mat& img,
			   const geo2d::Vector<float>& center,
			   const std::vector<float>& radius_v,
			   const float pi_threshold,
			   const float asup,
			   const float wsup)
  {
    LAROCV_SDEBUG() << " called with center " << center
		    << ", " << radius_v.size()
		    << " radii, px thresh " << pi_threshold
		    << ", & sup " << asup << std::endl;
    
    std::vector<geo2d::VectorArray<float> > res_v;
    geo2d::VectorArray<float> tmp_v;
    bool inside=false;
    
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
	
      auto ret_v = RadialIntersections(polarimg,geo2d::Circle<float>(center,radius),col,pi_threshold,asup,wsup);
      tmp_v.clear();
      tmp_v.reserve(ret_v.size());
      for(auto& ret : ret_v) {
	inside  = true;
	inside &= ((ret.y>0) and (ret.x>0));
	inside &= ((ret.y<img.rows) and (ret.x<img.cols));
	if (inside and img.at<uchar>(ret.y,ret.x)) 
	  tmp_v.emplace_back(std::move(ret));
      }
      res_v.emplace_back(std::move(tmp_v));
    }
    return res_v;
  }

  
  geo2d::VectorArray<float>
  RadialIntersections(const cv::Mat& polarimg,
		      const geo2d::Circle<float>& circle,
		      const int col,
		      const float pi_threshold,
		      const float asup,
		      const float wsup)
  {

    if (col > polarimg.cols) {
      LAROCV_SCRITICAL() << "Requested column " << col
			 << " outside max column " << polarimg.cols << std::endl;
      throw larbys();
    }
    std::vector<std::pair<int,int> > range_v;
    std::pair<int,int> range(-1,-1);
    
    for(int row=0; row<polarimg.rows; ++row) {
      
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
      float width = std::fabs(r.second - r.first);

      if(angle < 0) angle += (float)(polarimg.rows);

      angle *= (M_PI * 2. / ((float)(polarimg.rows)));
      width *= (M_PI * 2. / ((float)(polarimg.rows)));

      range_a_w_v.emplace_back(angle,width);
    }

    auto npts = range_a_w_v.size();
    
    if(asup>0 and npts>1) {
      
      auto dasup = asup * T2R;
      auto dwsup = wsup * T2R;
      LAROCV_SDEBUG() << "dasup=" << dasup << " dwsup=" << dwsup << std::endl;
      std::vector<std::pair<float,float> > tmp_v;
      tmp_v.reserve(range_a_w_v.size());

      if (npts==2) {
	LAROCV_SDEBUG() << "@(r1,r2)=("<<0<<","<<1<<") ";
	LAROCV_SDEBUG() << "(a1,a2)="<<range_a_w_v.front().first<<","<<range_a_w_v.back().first<<" ";
	LAROCV_SDEBUG() << "diff="<<std::fabs(range_a_w_v.front().first - range_a_w_v.back().first)<<std::endl;
	LAROCV_SDEBUG() << "(w1,w2)="<<range_a_w_v.front().second<<","<<range_a_w_v.back().second<<std::endl;
	auto da = std::fabs(range_a_w_v.front().first  - range_a_w_v.back().first);
	if (da < dasup) {
	  tmp_v.emplace_back(range_a_w_v.front().second > range_a_w_v.back().second ? range_a_w_v.front() : range_a_w_v.back());
	  std::swap(tmp_v,range_a_w_v);
	}
      }
      else if (npts>2) {
	std::set<size_t> preserved_s;
	std::set<size_t> modified_s;
	
	for(size_t r1=0; r1<npts; r1++) { 
	  for(size_t r2=r1+1; r2<npts; r2++) {
	    LAROCV_SDEBUG() << "@(r1,r2)=("<<r1<<","<<r2<<") ";
	    LAROCV_SDEBUG() << "(a1,a2)="<<range_a_w_v[r1].first<<","<<range_a_w_v[r2].first<<" ";
	    LAROCV_SDEBUG() << "diff="<<std::fabs(range_a_w_v[r1].first - range_a_w_v[r2].first)<<std::endl;
	    LAROCV_SDEBUG() << "(w1,w2)="<<range_a_w_v[r1].second<<","<<range_a_w_v[r2].second<<std::endl;
	    auto da = std::fabs(range_a_w_v[r1].first  - range_a_w_v[r2].first);
	    //auto dw = std::fabs(range_a_w_v[r1].second - range_a_w_v[r2].second);
	    if (da < dasup) {
	      size_t pidx = range_a_w_v[r1].second > range_a_w_v[r2].second ? r1 : r2;
	      size_t ridx = pidx == r1 ? r2 : r1;
	      preserved_s.insert(pidx);
	      modified_s.insert(pidx);
	      modified_s.insert(ridx);
	    }
	  }
	}
	for(auto pidx : preserved_s) 
	  tmp_v.emplace_back(range_a_w_v[pidx]);

	for(size_t r1=0; r1<npts; r1++) {
	  if (modified_s.find(r1) != modified_s.end()) continue;
	  tmp_v.emplace_back(range_a_w_v[r1]);
	}
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
      LAROCV_SDEBUG() << "..." << pt << std::endl;
    }
    return res;
  }

  geo2d::Vector<float> MeanPixel(const cv::Mat& img, const geo2d::Vector<float>& center,
				 size_t range_x, size_t range_y, float pi_threshold)
  {
    // Make a better guess
    cv::Rect rect(center.x - range_x, center.y - range_y, range_x*2+1,range_y*2+1);
    CorrectEdgeRectangle(img,rect,range_x*2+1,range_y*2+1);
    
    LAROCV_SDEBUG() << "rows cols " << img.rows
		    << "," << img.cols << " and rect " << rect << std::endl;
    auto small_img = cv::Mat(img,rect);

    auto thresh_small_img = Threshold(small_img,pi_threshold,1);
    
    auto points = FindNonZero(thresh_small_img);
    
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
  

  geo2d::Line<float> SquarePCA(const cv::Mat& img,
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
  SquareR(const cv::Mat& img,
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
  CorrectEdgeRectangle(const cv::Mat& img,
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
      LAROCV_SDEBUG() << "Wrote: " << ss.str() << std::endl;
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
      LAROCV_SDEBUG() << "Wrote: " << ss.str() << std::endl;
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
		       const float asup,
		       const float wsup) {
    
    auto xs_v = QPointOnCircle(img,circle,pi_threshold,asup,wsup);
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
		   const geo2d::Vector<float>& pt,
		   const int direction) {
    

    static std::array<geo2d::VectorArray<float>,4> cross_vv;
    static std::array<geo2d::Vector<float>,4> dir_v = {
      geo2d::Vector<float>( 1,  0),
      geo2d::Vector<float>( 0,  1),
      geo2d::Vector<float>(-1,  0),
      geo2d::Vector<float>( 0, -1),
    };
    

    for(size_t dir_id=0; dir_id<4; ++dir_id) {
      auto& cross_v = cross_vv[dir_id];
      cross_v.clear();
	
      auto dir = dir_v[dir_id];
      auto dir_pt = pt;
      dir_pt.x += 0.5;
      dir_pt.y += 0.5;

      int dir_ctr = 0;
      
      uint px = kINVALID_UINT;

      while(px and dir_ctr < 10 and Contained(img,dir_pt)) {
	cross_v.push_back(dir_pt);
	px = (uint) img.at<uchar>(dir_pt.y,dir_pt.x);
	dir_ctr++;
	dir_pt = pt + dir_ctr * dir;
      }
    }
      
    // get average pt in this direction
    auto res_cross0 = std::move(cross_vv[0]);
    res_cross0     += std::move(cross_vv[2]);
    auto res0 = res_cross0.mean();

    auto res_cross1 = std::move(cross_vv[1]);
    res_cross1     += std::move(cross_vv[3]);
    auto res1 = res_cross1.mean();

    auto res2 = geo2d::mean(res0,res1);

    if (!direction)  return res0;
    if (direction>0) return res1;
    if (direction<0) return res2;
    
    throw larbys("unspecified direction?");
    return res2;
  }
  
  geo2d::VectorArray<float> 
  OnCircleGroups(const cv::Mat& img,
		 const geo2d::Circle<float>& c) {
    
    auto ret_img = OnCircleImage(img,c);
    auto pts_v   = FindNonZero(ret_img);
    auto ctor_v  = FindContours(ret_img);

    std::vector<bool> used_v(pts_v.size(),false);

    static geo2d::VectorArray<float> res_v;
    res_v.clear();
    res_v.resize(ctor_v.size());
    geo2d::VectorArray<float> in_v;
    
    for(size_t cid=0; cid<ctor_v.size(); ++cid) {
      in_v.clear();
      in_v.reserve(pts_v.size() / 2);
      for(size_t pid=0; pid<pts_v.size(); ++pid) {
	if (used_v[pid]) continue;
	if (PointPolygonTest(ctor_v[cid],pts_v[pid])) {
	  in_v.emplace_back(pts_v[pid].x,pts_v[pid].y);
	  used_v[pid] = true;
	}
      }

      if (in_v.size() == 1) continue;

      res_v[cid] = geo2d::AngularAverage(c,in_v);
    }
    return res_v;
  }
    
  std::vector<geo2d::VectorArray<float> >
  OnCircleGroupsOnCircleArray(const cv::Mat& img,
			      const geo2d::Vector<float>& center,
			      const std::vector<float>& radius_v)
  {

    std::vector<geo2d::VectorArray<float> > res_v;
    res_v.resize(radius_v.size());
    
    for(size_t rid=0; rid<radius_v.size(); ++rid) 
      res_v[rid] = OnCircleGroups(img,geo2d::Circle<float>(center,radius_v[rid]));
    
    return res_v;
  }


}
#endif
