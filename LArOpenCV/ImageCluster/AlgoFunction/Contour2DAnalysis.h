#ifndef __CONTOUR2DANALYSIS_H_
#define __CONTOUR2DANALYSIS_H_

#include "LArOpenCV/Core/laropencv_logger.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "Geo2D/Core/Circle.h"
#include "Geo2D/Core/Line.h"

namespace larocv {

  cv::RotatedRect MinAreaRect(const GEO2D_Contour_t& ctor);

  double
  ArcLength(const GEO2D_Contour_t& ctor, bool closed=false);

  double
  ContourArea(const GEO2D_Contour_t& ctor,bool oriented=false);
  
  GEO2D_Contour_t
  ConvexHull(const GEO2D_Contour_t& ctor);
  
  GEO2D_ContourArray_t
  FindContours(const cv::Mat& img);

  GEO2D_ContourArray_t
  FindContours(const cv::Mat& img,
	       uint min_pix);
  
  cv::Mat
  CleanImage(const cv::Mat& img,
	     const GEO2D_ContourArray_t& veto_ctor_v,
	     float pi_threshold=0);

  cv::Mat
  MaskImage(const cv::Mat& img,
	    const GEO2D_ContourArray_t& veto_ctor_v,
	    int   tol=0,
	    bool  maskout=true);
  
  cv::Mat
  MaskImage(const cv::Mat& img,
	    const GEO2D_Contour_t& veto_ctor,
	    int   tol=0,
	    bool  maskout=true);
  
  cv::Mat
  MaskImage(const cv::Mat& img,
	    const geo2d::Circle<float>& c,
	    int   tol=0,
	    bool  maskout=true);

  uint
  CountNonZero(const cv::Mat& img,
	       const GEO2D_Contour_t& ctor,
	       uint tol=0);
  uint
  CountNonZero(const cv::Mat& img,
	       const geo2d::Circle<float>& circle,
	       uint tol=0);
  
  double
  Pt2PtDistance(const geo2d::Vector<float>& pt,
		const GEO2D_Contour_t& ctor);
  
  double
  Pt2PtDistance(const geo2d::Vector<float>& pt,
		const GEO2D_Contour_t& ctor,
		geo2d::Vector<float>& closest_pt);
  
  double
  Pt2PtDistance(const GEO2D_Contour_t& ctor1,
		const GEO2D_Contour_t& ctor2,
		geo2d::Vector<float>& closest_pt1,
		geo2d::Vector<float>& closest_pt2);
  
  double
  Pt2PtDistance(const GEO2D_Contour_t& ctor1,
		const GEO2D_Contour_t& ctor2);
  
  GEO2D_Contour_t
  Merge(const GEO2D_Contour_t& ctor1,
	const GEO2D_Contour_t& ctor2);
  
  GEO2D_Contour_t
  MergeAndRefine(const GEO2D_Contour_t& ctor1,
		 const GEO2D_Contour_t& ctor2,
		 const cv::Mat& img);
  
  GEO2D_Contour_t
  MergeByMask(const GEO2D_Contour_t& ctor1,
	      const GEO2D_Contour_t& ctor2,
	      const cv::Mat& img,
	      uint tol=0);
  
  geo2d::Line<float>
  CalcPCA(const GEO2D_Contour_t& ctor,
	  float EPS=1e-6);
  
  geo2d::Line<float>
  CalcPCA(const cv::Mat& img,
	  float EPS=1e-6);
  
  geo2d::Line<float>
  PrinciplePCA(const cv::Mat& row_mat,
	       float EPS=0);
  
  double
  AreaOverlap(const GEO2D_Contour_t& ctr0,
	      const GEO2D_Contour_t& ctr1);

  size_t
  FindContainingContour(const GEO2D_ContourArray_t& contour_v,
			const GEO2D_Contour_t& ctr);

  size_t
  FindContainingContour(const GEO2D_ContourArray_t& contour_v,
			const geo2d::Vector<float>& pt,
			double& distance);
  
  size_t
  FindContainingContour(const GEO2D_ContourArray_t& contour_v,
			const geo2d::Vector<float>& pt);

  double
  PixelFraction(const cv::Mat& img,
		const GEO2D_Contour_t& super_ctor,
		const GEO2D_Contour_t& target_ctor);

  double
  PixelFraction(const cv::Mat& img,
		const GEO2D_Contour_t& super_ctor,
		const GEO2D_ContourArray_t& target_ctor_v);

  double
  CircumferenceAngularSum(const GEO2D_Contour_t& ctor,
			  bool isclosed=false);

  void
  FindEdges(const GEO2D_Contour_t& ctor,
	    geo2d::Vector<float>& edge1,
	    geo2d::Vector<float>& edge2);

  GEO2D_ContourArray_t
  AddContourArrays(const GEO2D_ContourArray_t& ctor_arr_1,
		   const GEO2D_ContourArray_t& ctor_arr_2);
  
}
#endif
