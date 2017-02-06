#ifndef __CONTOUR2DANALYSIS_H_
#define __CONTOUR2DANALYSIS_H_

#include "LArOpenCV/Core/laropencv_logger.h"
#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "Geo2D/Core/Circle.h"
#include "Geo2D/Core/Line.h"

namespace larocv {

  cv::Mat CleanImage(const cv::Mat& img,
		     const GEO2D_ContourArray_t& veto_ctor_v,
		     float pi_threshold=0);


  cv::Mat MaskImage(const cv::Mat& img,
		    const GEO2D_ContourArray_t& veto_ctor_v,
		    int   tol=0,
		    bool  maskout=true);

  cv::Mat MaskImage(const cv::Mat& img,
		    const GEO2D_Contour_t& veto_ctor,
		    int   tol=0,
		    bool  maskout=true);

  cv::Mat MaskImage(const cv::Mat& img,
		    const geo2d::Circle<float>& c,
		    int   tol=0,
		    bool  maskout=true);
  
  double Pt2PtDistance(const geo2d::Vector<float>& pt,
		       const GEO2D_Contour_t& ctor);

  double Pt2PtDistance(const geo2d::Vector<float>& pt,
		       const GEO2D_Contour_t& ctor,
		       geo2d::Vector<float>& closest_pt);

  double Pt2PtDistance(const GEO2D_Contour_t& ctor1,
		       const GEO2D_Contour_t& ctor2,
		       geo2d::Vector<float>& closest_pt1,
		       geo2d::Vector<float>& closest_pt2);

  double Pt2PtDistance(const GEO2D_Contour_t& ctor1,
		       const GEO2D_Contour_t& ctor2);

  GEO2D_Contour_t Merge(const GEO2D_Contour_t& ctor1,
			const GEO2D_Contour_t& ctor2);

  GEO2D_Contour_t MergeAndRefine(const GEO2D_Contour_t& ctor1,
				 const GEO2D_Contour_t& ctor2,
				 const cv::Mat& img);
  
  GEO2D_Contour_t MergeByMask(const GEO2D_Contour_t& ctor1,
			      const GEO2D_Contour_t& ctor2,
			      const cv::Mat& img);
  
  
  geo2d::Line<float> CalcPCA(const GEO2D_Contour_t& ctor,float EPS=1e-6);
  
}
#endif
