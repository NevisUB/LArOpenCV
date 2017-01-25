#ifndef __CONTOUR2DANALYSIS_CXX_
#define __CONTOUR2DANALYSIS_CXX_

#include "Contour2DAnalysis.h"
#include "Core/VectorArray.h"
#include "opencv2/imgproc.hpp"

namespace larocv {

  cv::Mat CleanImage(const cv::Mat& img,
		     const GEO2D_ContourArray_t& veto_ctor_v,
		     float pi_threshold)
  {
    cv::Mat thresh_img;
    ::cv::threshold(img,thresh_img,pi_threshold,1000,3);
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
		    const GEO2D_ContourArray_t& veto_ctor_v,
		    float pi_threshold,
		    bool  maskout)
  {
    cv::Mat thresh_img;
    cv::threshold(img,thresh_img,pi_threshold,1000,3);

    cv::Mat mask = cv::Mat(img.size(), img.type(), 0);
    cv::drawContours(mask, veto_ctor_v, -1, cv::Scalar(255), CV_FILLED, cv::LINE_8);

    if(maskout) cv::bitwise_not(mask,mask);

    thresh_img.copyTo(thresh_img,mask);

    return thresh_img;
  }
  
  cv::Mat MaskImage(const cv::Mat& img,
		    const GEO2D_ContourArray_t& veto_ctor_v,
		    int   tol,
		    bool  maskout)

  {
    cv::Mat dst_img(img.size(),img.type());

    cv::Mat mask = cv::Mat(img.size(), img.type(), CV_8UC1);
    cv::drawContours(mask, veto_ctor_v, -1, cv::Scalar(255), -1, cv::LINE_8); // fill inside
    if (tol > 0)
      cv::drawContours(mask, veto_ctor_v, -1, cv::Scalar(255), tol, cv::LINE_8); // make the edges thicker to mask outwards

    //invert mask
    if(maskout) cv::bitwise_not(mask,mask);

    //for some reason, input image and masked image cannot be the same from vic's test
    img.copyTo(dst_img,mask);

    return dst_img;
  }
  
  cv::Mat MaskImage(const cv::Mat& img,
		    const geo2d::Circle<float>& c,
		    float pi_threshold,
		    bool  maskout)
  {
    cv::Mat thresh_img;
    cv::threshold(img,thresh_img,pi_threshold,1000,3);
    
    cv::Mat mask = cv::Mat(img.size(),img.type(),0);
    cv::circle(mask, // output
	       cv::Point( (int)(c.center.x+0.5),(int(c.center.y+0.5))), // center
	       (int)(c.radius), // radius
	       cv::Scalar(255), // single channel
	       -1, // color filled
	       ::cv::LINE_8, // type of boundary
	       0);
    thresh_img.copyTo(thresh_img,mask);

    return thresh_img;
  }

  double Distance(const geo2d::Vector<float>& pt,
		  const GEO2D_Contour_t& ctor)
  {
    static geo2d::Vector<float> closest_pt;
    return Distance(pt,ctor,closest_pt);
  }

  double Distance(const geo2d::Vector<float>& pt,
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

  double Distance(const GEO2D_Contour_t& ctor1,
		  const GEO2D_Contour_t& ctor2)
  {
    static geo2d::Vector<float> pt1;
    static geo2d::Vector<float> pt2;
    return Distance(ctor1,ctor2,pt1,pt2);
  }

  double Distance(const GEO2D_Contour_t& ctor1,
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


  geo2d::Line<float> CalcPCA(const GEO2D_Contour_t& ctor) {
    
    cv::Mat ctor_pts(ctor.size(), 2, CV_32FC1); // 32 bit precision is fine
    
    for (unsigned i = 0; i < ctor_pts.rows; ++i) {
      ctor_pts.at<float>(i, 0) = ctor.at(i).x;
      ctor_pts.at<float>(i, 1) = ctor.at(i).y;
    }
    
    cv::PCA pca_ana(ctor_pts, cv::Mat(), CV_PCA_DATA_AS_ROW,0);
      
    geo2d::Line<float> pca_principle(geo2d::Vector<float>(pca_ana.mean.at<float>(0,0),
							  pca_ana.mean.at<float>(0,1)),
				     geo2d::Vector<float>(pca_ana.eigenvectors.at<float>(0,0),
							  pca_ana.eigenvectors.at<float>(0,1)));
    return pca_principle;
  }

  
  
}
#endif
