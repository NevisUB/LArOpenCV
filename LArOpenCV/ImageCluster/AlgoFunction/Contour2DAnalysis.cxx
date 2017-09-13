#ifndef __CONTOUR2DANALYSIS_CXX_
#define __CONTOUR2DANALYSIS_CXX_

#include "Contour2DAnalysis.h"
#include "Geo2D/Core/VectorArray.h"
#ifndef __CLING__
#ifndef __CINT__
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#endif
#endif
#include "LArOpenCV/Core/larbys.h"
#include "ImagePatchAnalysis.h"

namespace larocv {

  bool
  PointPolygonTest(const GEO2D_Contour_t& ctor,
		   const geo2d::Vector<float>& pt) {

    static double dist;
    return PointPolygonTest(ctor,pt,dist);
  }
  
  bool
  PointPolygonTest(const GEO2D_Contour_t& ctor,
		   const geo2d::Vector<float>& pt,
		   double& dist) {
    dist = cv::pointPolygonTest(ctor,pt,true);
    if (dist >= 0)
      return true;

    dist *= -1;
    return false;
  }

  
  cv::RotatedRect
  MinAreaRect(const GEO2D_Contour_t& ctor) {
    return cv::minAreaRect(ctor);
  }

  double
  ArcLength(const GEO2D_Contour_t& ctor, bool closed) {
    return cv::arcLength(ctor,closed);
  }

  double
  ContourArea(const GEO2D_Contour_t& ctor,bool oriented) {
    return cv::contourArea(ctor,oriented);
  }
  

  double
  ContourArea(const cv::Mat& img,
	      const GEO2D_Contour_t& ctor){
    auto white_img = BlankImage(img);
    white_img = MaskImage(white_img,ctor,0,false);
    return (double)CountNonZero(white_img);
  }


  GEO2D_Contour_t
  ConvexHull(const GEO2D_Contour_t& ctor) {
    GEO2D_Contour_t hull_ctor;
    cv::convexHull(ctor,hull_ctor);
    return hull_ctor;
  }
  
  GEO2D_ContourArray_t
  FindContours(const cv::Mat& img)
  {
    auto img_copy = img.clone();
    GEO2D_ContourArray_t result_v;
    std::vector<cv::Vec4i> cv_hierarchy_v;
    cv::findContours(img_copy,result_v,cv_hierarchy_v,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
    return result_v;
  }


  GEO2D_ContourArray_t
  FindContours(const cv::Mat& img,uint min_pix)
  {
    auto ctor_v = FindContours(img);
    GEO2D_ContourArray_t res_v;
    res_v.reserve(ctor_v.size());
    
    for(auto& ctor : ctor_v) {
      if (CountNonZero(img,ctor,0) < min_pix) continue;
      res_v.emplace_back(std::move(ctor));
    }
    
    return res_v;
  }
    
  cv::Mat
  CleanImage(const cv::Mat& img,
	     const GEO2D_ContourArray_t& veto_ctor_v,
	     float pi_threshold)
  {
    cv::Mat thresh_img;
    cv::threshold(img,thresh_img,pi_threshold,1000,3);
    geo2d::VectorArray<int> points;
    findNonZero(thresh_img, points);
    for(auto const& pt : points) {
      for(auto const& veto_ctor : veto_ctor_v) {
	auto dist = ::cv::pointPolygonTest(veto_ctor,pt,false);
	if(dist<0) continue;
	thresh_img.at<unsigned char>(pt.y,pt.x) = 0;
	break;
      }
    }
    return thresh_img;
  }

  cv::Mat MaskImage(const cv::Mat& img,
		    const GEO2D_Contour_t& veto_ctor,
		    int   tol,
		    bool  maskout)
  {
    GEO2D_ContourArray_t veto_ctor_v(1,veto_ctor);
    return MaskImage(img, veto_ctor_v, tol, maskout);
  }
  
  cv::Mat MaskImage(const cv::Mat& img,
		    const GEO2D_ContourArray_t& veto_ctor_v,
		    int   tol,
		    bool  maskout)

  {
    //cv::Mat dst_img(img.size(),img.type(),CV_8UC1);
    //cv::Mat mask = cv::Mat(img.size(), img.type(), CV_8UC1);

    cv::Mat dst_img(img.size(),img.type(),cv::Scalar(0));
    cv::Mat mask(img.size(), img.type(),cv::Scalar(0));
    
    cv::drawContours(mask, veto_ctor_v, -1, cv::Scalar(255), -1, cv::LINE_8); // fill inside
    if (tol > 0) 
      cv::drawContours(mask, veto_ctor_v, -1, cv::Scalar(255), tol, cv::LINE_8); // make the edges thicker to mask outwards


    //invert mask
    if(maskout) cv::bitwise_not(mask,mask);

    img.copyTo(dst_img,mask);

    return dst_img;
  }
  
  cv::Mat MaskImage(const cv::Mat& img,
		    const geo2d::Circle<float>& c,
		    int  tol,
		    bool  maskout)
  {
    // cv::Mat dst_img(img.size(),img.type(), CV_8UC1);
    // cv::Mat mask = cv::Mat(img.size(),img.type(),CV_8UC1);

    cv::Mat dst_img(img.size(),img.type(),cv::Scalar(0));
    cv::Mat mask(img.size(), img.type(),cv::Scalar(0));

    cv::circle(mask, // output
	       cv::Point( (int)(c.center.x+0.5),(int(c.center.y+0.5))), // center
	       (int)(c.radius), // radius
	       cv::Scalar(255), // single channel
	       -1, // color filled
	       ::cv::LINE_8, // type of boundary
	       0);
    if(tol>0)
      cv::circle(mask, // output
		 cv::Point( (int)(c.center.x+0.5),(int(c.center.y+0.5))), // center
		 (int)(c.radius), // radius
		 cv::Scalar(255), // single channel
		 tol, // color filled
		 ::cv::LINE_8, // type of boundary
		 0);

    if(maskout) cv::bitwise_not(mask,mask);

    img.copyTo(dst_img,mask);
    return dst_img;
  }


  uint
  CountNonZero(const cv::Mat& img,
	       const GEO2D_Contour_t& ctor,
	       uint tol){
    return cv::countNonZero(MaskImage(img,ctor,tol,false));
  }

  uint
  CountNonZero(const cv::Mat& img,
	       const geo2d::Circle<float>& circle,
	       uint tol){
    return cv::countNonZero(MaskImage(img,circle,tol,false));
  }
  
  double Pt2PtDistance(const geo2d::Vector<float>& pt,
		       const GEO2D_Contour_t& ctor)
  {
    static geo2d::Vector<float> closest_pt;
    return Pt2PtDistance(pt,ctor,closest_pt);
  }

  double Pt2PtDistance(const geo2d::Vector<float>& pt,
		       const GEO2D_Contour_t& ctor,
		       geo2d::Vector<float>& closest_pt)
  {
    double min_dist=1.e20;
    double dist,dx,dy;
    for(auto const& cand_pt : ctor) {
      dx = cand_pt.x - pt.x;
      if(dx > min_dist) continue;
      dy = cand_pt.y - pt.y;
      if(dy > min_dist) continue;
      dist = pow(dx,2) + pow(dy,2);
      if(dist > min_dist) continue;
      min_dist = dist;
      closest_pt = pt;
    }
    return sqrt(min_dist);
  }

  double Pt2PtDistance(const GEO2D_Contour_t& ctor1,
		       const GEO2D_Contour_t& ctor2)
  {
    static geo2d::Vector<float> pt1;
    static geo2d::Vector<float> pt2;
    return Pt2PtDistance(ctor1,ctor2,pt1,pt2);
  }

  double Pt2PtDistance(const GEO2D_Contour_t& ctor1,
		       const GEO2D_Contour_t& ctor2,
		       geo2d::Vector<float>& closest_pt1,
		       geo2d::Vector<float>& closest_pt2)
  {
    double min_dist=1.e20;
    double dist,dx,dy;
    for(auto const& pt1 : ctor1) {
      for(auto const& pt2 : ctor2) {
	dx = pt1.x - pt2.x;
	if(dx > min_dist) continue;
	dy = pt1.y - pt2.y;
	if(dy > min_dist) continue;
	dist = pow(dx,2)+pow(dy,2);
	if(dist > min_dist) continue;
	closest_pt1 = pt1;
	closest_pt2 = pt2;
	min_dist = dist;
      }
    }
    return sqrt(min_dist);
  }

  GEO2D_Contour_t Merge(const GEO2D_Contour_t& ctor1,
			const GEO2D_Contour_t& ctor2)
  {
    GEO2D_Contour_t result;
    GEO2D_Contour_t merged_ctor;
    merged_ctor.reserve(ctor1.size()+ctor2.size());
    merged_ctor.insert(merged_ctor.end(), ctor1.begin(), ctor1.end());
    merged_ctor.insert(merged_ctor.end(), ctor2.begin(), ctor2.end());

    cv::convexHull(cv::Mat(merged_ctor), result);
    
    return result;
  }

  GEO2D_Contour_t MergeAndRefine(const GEO2D_Contour_t& ctor1,
				 const GEO2D_Contour_t& ctor2,
				 const cv::Mat& img)
  {
    LAROCV_SDEBUG() << "Merge and refine..." << std::endl;
    LAROCV_SDEBUG() << "... received 2 contours sizes " << ctor1.size() << ", " << ctor2.size() << std::endl;
    auto result = Merge(ctor1,ctor2);
    auto masked_img = MaskImage(img,result,0,false);
    
    auto result_v = FindContours(masked_img);
    LAROCV_SDEBUG() << "... found " << result_v.size() << " contours in mask" << std::endl;
    
    if (!result_v.size()) {
      
      throw larbys("No refined contour can be determined");
    }
    if (result_v.size()==1) {
      LAROCV_SDEBUG() << "... returning contour of size " << result_v[0].size() << std::endl;
      return result_v[0];
    }
    
    uint size=0;
    int idx=-1;
    for(uint i=0;i<result_v.size();++i) {
      if (result_v.size() > size)
	{ idx=i; size=result_v.size(); }
    }
    
    if(idx<0) throw larbys("No refined contour can be determined");

    LAROCV_SDEBUG() << "... returning contour of size " << result_v[idx].size() << std::endl;
    
    return result_v[idx];
  }


  GEO2D_Contour_t MergeByMask(const GEO2D_Contour_t& ctor1,
			      const GEO2D_Contour_t& ctor2,
			      const cv::Mat& img,
			      uint tol)
  {

    LAROCV_SDEBUG() << "Merging by mask..." << std::endl;
      
    GEO2D_ContourArray_t ctor_v;
    ctor_v.resize(2);
    ctor_v[0] = ctor1;
    ctor_v[1] = ctor2;
    LAROCV_SDEBUG() << "... received 2 contours sizes " << ctor1.size() << ", " << ctor2.size() << std::endl;
    
    auto masked_img = MaskImage(img,ctor_v,tol,false);

    auto result_v = FindContours(masked_img);
    
    LAROCV_SDEBUG() << "... found " << result_v.size() << " contours in mask" << std::endl;
    
    if (!result_v.size()) throw larbys("No contours found in masked image");
    if (result_v.size()==1) {
      LAROCV_SDEBUG() << "... returning contour of size " << result_v[0].size() << std::endl;
      return result_v[0];
    }
    
    size_t max_npx=0;
    int idx=-1;
    for(uint i=0;i<result_v.size();++i) {
      const auto& ctor = result_v[i];
      auto npx = CountNonZero(masked_img,ctor);
      LAROCV_SDEBUG() << "("<<i<<") w/ npx "<<npx<<std::endl;
      if ( npx > max_npx ) {
	max_npx = npx;
	idx=i;
      }
    }
    
    if( idx < 0) throw larbys("No index found, refined contour can be determined.");

    LAROCV_SDEBUG()<< "... returning ("<<idx<<") of size "<<result_v[idx].size()<<" with npx "<<max_npx<<std::endl;
    
    return result_v[idx];
  }
  

  geo2d::Line<float> CalcPCA(const GEO2D_Contour_t& ctor,
			     float EPS) {
    
    if(ctor.size() < 2) {
      //LAROCV_SWARNING() << "PCA approx cannot be made (# points " << ctor.size() << " < 2)" << std::endl;
      throw larbys();
    }
    
    cv::Mat mat(ctor.size(), 2, CV_32FC1);
    
    for (unsigned i = 0; i < mat.rows; ++i) {
      mat.at<float>(i, 0) = ctor[i].x;
      mat.at<float>(i, 1) = ctor[i].y;
    }

    return PrinciplePCA(mat,EPS);
    
  }
  
  geo2d::Line<float> CalcPCA(const cv::Mat& img,
			     float EPS) {


    auto pts_v = FindNonZero(img);
    
    if(pts_v.size() < 2) {
      //LAROCV_SWARNING() << "PCA approx cannot be made (# points " << pts_v.size() << " < 2)" << std::endl;
      throw larbys();
    }
    
    cv::Mat mat(pts_v.size(), 2, CV_32FC1);
    
    for (unsigned i = 0; i < mat.rows; ++i) {
      mat.at<float>(i, 0) = pts_v[i].x;
      mat.at<float>(i, 1) = pts_v[i].y;
    }

    return PrinciplePCA(mat,EPS);
  }
  
  geo2d::Line<float> PrinciplePCA(const cv::Mat& row_mat,
				  float EPS) {

    if (EPS<=0) throw larbys("Function called w/o EPS specified. Consider calling CalcPCA(...)!");
    cv::PCA pca_ana(row_mat, cv::Mat(), CV_PCA_DATA_AS_ROW,0);
    
    auto meanx=pca_ana.mean.at<float>(0,0);
    auto meany=pca_ana.mean.at<float>(0,1);
    
    auto eigenPx=pca_ana.eigenvectors.at<float>(0,0);
    auto eigenPy=pca_ana.eigenvectors.at<float>(0,1);
    
    LAROCV_SDEBUG() << "meanx : " << meanx << "... meany: " << meany << "... ePx: " << eigenPx << "... ePy: " << eigenPy << std::endl;
    
    if (eigenPx==0) { 
      LAROCV_SDEBUG() << "Invalid Px inf detected set PX=EPS" << std::endl;
      eigenPx=EPS;
    }
    
    return geo2d::Line<float>(geo2d::Vector<float>(meanx,meany),
			      geo2d::Vector<float>(eigenPx,eigenPy));
    
  }


  double
  AreaOverlap(const GEO2D_Contour_t& ctr0,
	      const GEO2D_Contour_t& ctr1)
  {
    int rows, cols;
    rows = cols = 0;
    for(auto const& pt : ctr0) {
      if(rows < pt.y) rows = (int)(pt.y)+1;
      if(cols < pt.x) cols = (int)(pt.x)+1;
    }
    for(auto const& pt : ctr1) {
      if(rows < pt.y) rows = (int)(pt.y)+1;
      if(cols < pt.x) cols = (int)(pt.x)+1;
    }
    cv::Mat res(rows,cols,CV_8UC1,cv::Scalar(255));
    res = MaskImage(res,ctr0,0,false);
    res = MaskImage(res,ctr1,0,false);

    return (double)cv::countNonZero(res);
  }

  size_t
  FindContainingContour(const GEO2D_ContourArray_t& contour_v,
			const GEO2D_Contour_t& ctr)
  {
    size_t res = kINVALID_SIZE;
    double max_area = -1.0*kINVALID_DOUBLE;
    for(size_t idx = 0; idx < contour_v.size(); ++idx) {
      auto area_overlap = AreaOverlap(contour_v.at(idx),ctr);
      if(area_overlap >= max_area) {
	max_area = area_overlap;
	res = idx;
      }
    }
    return res;
  }

  size_t
  FindContainingContour(const GEO2D_ContourArray_t& contour_v,
			const geo2d::Vector<float>& pt,
			double& distance) {
    distance=kINVALID_SIZE;
    auto idx = FindContainingContour(contour_v,pt);
    if (idx==kINVALID_SIZE) return kINVALID_SIZE;
    distance = (double)cv::pointPolygonTest(contour_v.at(idx),pt,true);
    return idx;
  }
  
  size_t
  FindContainingContour(const GEO2D_ContourArray_t& contour_v,
			const geo2d::Vector<float>& pt)
  {
    LAROCV_SDEBUG() << "Recieved " << contour_v.size() << " contours w/ pt " << pt << std::endl;
    size_t parent_ctor_id   = kINVALID_SIZE;
    size_t parent_ctor_size = 0;
    double dist2vtx = -1e9;
    for(size_t ctor_id=0; ctor_id < contour_v.size(); ++ctor_id){
      auto const& ctor = contour_v[ctor_id];
      LAROCV_SDEBUG() << "ctor id: " << ctor_id << std::endl;
      auto dist = cv::pointPolygonTest(ctor, pt, true);
      LAROCV_SDEBUG() << "    dist: " << dist << std::endl;
      if(dist < dist2vtx) continue;
      if(dist2vtx >= 0 && parent_ctor_size > ctor.size()) continue;
      parent_ctor_id = ctor_id;
      parent_ctor_size = ctor.size();
      dist2vtx = dist;
      LAROCV_SDEBUG() << "    size: " << ctor.size() << std::endl;
    }
    LAROCV_SINFO() << "Vertex @ " << pt << " belongs to super cluster id " << parent_ctor_id << std::endl;
    return parent_ctor_id;
  }  

  double
  PixelFraction(const cv::Mat& img,
		const GEO2D_Contour_t& super_ctor,
		const GEO2D_ContourArray_t& target_ctor_v) {

    // mask this contour from the image
    auto super_img = MaskImage(img,super_ctor,0,false);

    //count the number of super pixels
    double super_px = cv::countNonZero(super_img);

    if (super_px == 0) {
      LAROCV_SWARNING() << "Number of super pixels in image is zero!" << std::endl;
      LAROCV_SWARNING() << "super ctor size " << super_ctor.size();
      return 0.0;
      //throw larbys();
    }
    
    //count the number of target pixels in this image
    double target_px = 0;
    for(auto const& target_ctor : target_ctor_v) 
      target_px += cv::countNonZero(MaskImage(super_img,target_ctor,0,false));

    LAROCV_SDEBUG() << "...got " << super_px << " and " << target_px << " frac " << target_px/super_px << std::endl;
    return target_px / super_px;
  }

  double PixelFraction(const cv::Mat& img,const GEO2D_Contour_t& super_ctor, const GEO2D_Contour_t& target_ctor) {
    GEO2D_ContourArray_t target_ctor_v(1,target_ctor);
    return PixelFraction(img,super_ctor,target_ctor_v);
  }

  double
  CircumferenceAngularSum(const GEO2D_Contour_t& ctor,
			  bool isclosed) {
    if (ctor.empty()) {
      LAROCV_SWARNING() << "Cannot calculate angular sum for empty contour" << std::endl;
      throw larbys();
    }

    uint last_id = ctor.size();

    if (isclosed) {
      last_id=-1;
      if (ctor.front() != ctor.back())
	throw larbys("Caller specified ctor is closed, but it's not");
    }
    
    double circum = 0.0;
    double angle_sum = 0.0;
    double weight_sum = 0.0;
    double rad2deg = 57.2957795131;
    
    for(int cid=0; cid < last_id; ++cid) {
      int id1=cid;
      int id0=id1-1;
      int id2=id1+1;

      if(id0 < 0) id0 = last_id-1;
      if(id2 >= last_id) id2 = 0;
      
      geo2d::Vector<float> pt0(ctor[id0]);
      geo2d::Vector<float> pt1(ctor[id1]);
      geo2d::Vector<float> pt2(ctor[id2]);

      auto d21 = geo2d::dist(pt1,pt2);

      circum+=d21;
      
      auto pt10 = pt0-pt1;
      auto pt21 = pt2-pt1;

      //for now use this
      double angle = std::acos( pt21.dot(pt10) / (geo2d::length(pt10) * geo2d::length(pt21))); 
      angle *= rad2deg;

      angle_sum += angle / d21;
      weight_sum += 1/d21;
      circum += d21;
    }

    LAROCV_SDEBUG() << "Scanned contour size " << last_id << " of circumference " << circum << std::endl;
    return angle_sum / weight_sum;
  }
  
  void
  FindEdges(const GEO2D_Contour_t& ctor,
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

  void
  FindEdge(const GEO2D_Contour_t& ctor,
	   geo2d::Vector<float> edge1,
	   geo2d::Vector<float>& edge2)
  {
    // find the furthest point from edge1
    geo2d::Vector<float> ctor_pt;
    double dist_max=0;
    double dist;
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
  
  
  GEO2D_ContourArray_t AddContourArrays(const GEO2D_ContourArray_t& ctor_arr_1,
					const GEO2D_ContourArray_t& ctor_arr_2) {
    GEO2D_ContourArray_t ctors;
    ctors.reserve(ctor_arr_1.size() + ctor_arr_2.size());
    
    ctors.insert( ctors.end(), ctor_arr_1.begin(), ctor_arr_1.end());
    ctors.insert( ctors.end(), ctor_arr_2.begin(), ctor_arr_2.end());
    
    return ctors;
  }


}
#endif
