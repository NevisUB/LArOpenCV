#ifndef __CONTOUR2DANALYSIS_H_
#define __CONTOUR2DANALYSIS_H_

#include "Base/ImageClusterTypes.h"
#include "Core/Circle.h"
#include "Core/Line.h"

namespace larocv {

  cv::Mat CleanImage(const cv::Mat& img,
		     const GEO2D_ContourArray_t& veto_ctor_v,
		     float pi_threshold=0);


  cv::Mat MaskImage(const cv::Mat& img,
		    const GEO2D_ContourArray_t& veto_ctor_v,
		    int   tol=0,
		    bool  maskout=true);

  
  cv::Mat MaskImage(const cv::Mat& img,
		    const GEO2D_ContourArray_t& veto_ctor_v,
		    float pi_threshold=0,
		    bool  maskout=true);

  cv::Mat MaskImage(const cv::Mat& img,
		    const GEO2D_Contour_t& veto_ctor,
		    float pi_threshold=0,
		    bool  maskout=true);

  cv::Mat MaskImage(const cv::Mat& img,
		    const geo2d::Circle<float>& c,
		    float pi_threshold=0,
		    bool  maskout=true);
  
  double Distance(const geo2d::Vector<float>& pt,
		  const GEO2D_Contour_t& ctor);

  double Distance(const geo2d::Vector<float>& pt,
		  const GEO2D_Contour_t& ctor,
		  geo2d::Vector<float>& closest_pt);

  double Distance(const GEO2D_Contour_t& ctor1,
		  const GEO2D_Contour_t& ctor2,
		  geo2d::Vector<float>& closest_pt1,
		  geo2d::Vector<float>& closest_pt2);

  double Distance(const GEO2D_Contour_t& ctor1,
		  const GEO2D_Contour_t& ctor2);

  GEO2D_Contour_t Merge(const GEO2D_Contour_t& ctor1,
			const GEO2D_Contour_t& ctor2);
  
  geo2d::Line<float> CalcPCA(const GEO2D_Contour_t& ctor);
  
}
#endif
