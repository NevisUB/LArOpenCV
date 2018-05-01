#ifndef __IMAGEPATCHANALYSIS_H_
#define __IMAGEPATCHANALYSIS_H_

#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include <vector>
#include "Geo2D/Core/VectorArray.h"
#include "Geo2D/Core/Vector.h"
#include "Geo2D/Core/Circle.h"
#include "Geo2D/Core/Line.h"

namespace larocv {

  cv::Mat
  Flip(const cv::Mat& img, int flipCode);
  
  cv::Mat
  Transpose(const cv::Mat& img);
  
  cv::Mat
  BlankImage(const cv::Mat& img,uint val=255);

  cv::Mat
  LinearPolar(const cv::Mat& img,
	      geo2d::Vector<float> pt,
	      float radius);
  
  GEO2D_Contour_t
  FindNonZero(const cv::Mat& img);

  size_t
  CountNonZero(const cv::Mat& img);

  float
  SumNonZero(const cv::Mat& img);
  
  double
  MeanDistanceToLine(const cv::Mat& img,
		     const geo2d::Line<float>& line);

  double
  SigmaDistanceToLine(const cv::Mat& img,
		      const geo2d::Line<float>& line);
  
  cv::Mat
  Threshold(const cv::Mat& mat,
	    double thresh,
	    double max_val);

  cv::Mat 
  SmallImg(const cv::Mat& img,
	   const geo2d::Vector<float>& pt,
	   const float dx,
	   const float dy);

  geo2d::VectorArray<float>
  QPointOnCircle(const cv::Mat& img,
		 const geo2d::Circle<float>& circle,
		 const float pi_threshold=0.1,
		 const float asup=0,
		 const float wsup=0);
  
  std::vector<geo2d::VectorArray<float> >
  QPointArrayOnCircleArray(const cv::Mat& img,
			   const geo2d::Vector<float>& center,
			   const std::vector<float>& radius_v,
			   const float pi_threshold=0.1,
			   const float asup=0,
			   const float wsup=0);
  
  geo2d::VectorArray<float>
  RadialIntersections(const cv::Mat& polarimg,
		      const geo2d::Circle<float>& circle,
		      const int col,
		      const float pi_threshold,
		      const float asup=0,
		      const float wsup=0);

  geo2d::Vector<float>
  MeanPixel(const cv::Mat& img,
	    const geo2d::Vector<float>& center,
	    size_t range_x,
	    size_t range_y,
	    float pi_threshold=0);

  void
  FindEdges(const GEO2D_Contour_t& ctor,
	    geo2d::Vector<float>& edge1,
	    geo2d::Vector<float>& edge2);

  geo2d::Line<float>
  SquarePCA(const cv::Mat& img,
	    geo2d::Vector<float> pt,
	    float width, float height);

  double
  SquareR(const cv::Mat& img,
	  geo2d::Vector<float> pt,
	  float width, float height);
  
  void
  CorrectEdgeRectangle(const cv::Mat& img,
		       cv::Rect& rect,
		       int w,
		       int h);

  cv::Mat
  MaskImage(const cv::Mat& img,
	    const cv::Rect& rec,
	    int tol=0,
	    bool maskout=true);

  bool
  PathExists(const cv::Mat& img,
	     const geo2d::Vector<float>& pt1,
	     const geo2d::Vector<float>& pt2,
	     float dthresh=5.0,
	     float pthresh=0.0,
	     uint  min_ctor_size=0.0);
  float
  CircleDensity(const cv::Mat& img,
		const geo2d::Circle<float>& circle);
  
  float
  CircleDensity(const cv::Mat& img,
		const cv::Mat& white_img,
		const geo2d::Circle<float>& circle);
  
  bool
  ChargeBlobCircleEstimate(const cv::Mat& img,
			   const geo2d::Vector<float>& center,
			   geo2d::Circle<float>& res,
			   float thresh,
			   float start_rad,
			   float end_rad,
			   float step);

  geo2d::VectorArray<float>
  QPointOnCircleRefine(const cv::Mat& img,
		       const geo2d::Circle<float>& circle,
		       const geo2d::VectorArray<float>& xs_v,
		       const float mask_inner);


  geo2d::VectorArray<float>
  QPointOnCircleRefine(const cv::Mat& img,
		       const geo2d::Circle<float>& circle,
		       const float mask_inner,
		       const float pi_threshold,
		       const float asup,
		       const float wsup);
  bool
  NonZeroFloor(const cv::Mat& img,
               const geo2d::Vector<float>& pt);
  bool
  NonZeroCiel(const cv::Mat& img,
              const geo2d::Vector<float>& pt);

  bool
  Contained(const cv::Mat& img,
	    const geo2d::Vector<float>& pt);

  geo2d::Vector<float>
  Centroid(const cv::Mat& mat);

  geo2d::Vector<float>
  Centroid(const GEO2D_Contour_t& ctor);

  geo2d::Vector<float>
  EstimateMidPoint(const cv::Mat& img,
		   const geo2d::Vector<float>& pt,
		   const int direction);

  geo2d::VectorArray<float> 
  OnCircleGroups(const cv::Mat& img,
		 const geo2d::Circle<float>& c);

  std::vector<geo2d::VectorArray<float> >
  OnCircleGroupsOnCircleArray(const cv::Mat& img,
			      const geo2d::Vector<float>& center,
			      const std::vector<float>& radius_v);

  bool 
  Connected(const cv::Mat& img,
	    geo2d::Vector<float> pt1,
	    geo2d::Vector<float> pt2,
	    int thickness=1);

  bool 
  Broken(const cv::Mat& img,
         geo2d::Vector<float> pt1,
         geo2d::Vector<float> pt2,
         int thickness);
  }
#endif
