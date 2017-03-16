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
  BlankImage(const cv::Mat& img,uint val=255);
  
  GEO2D_Contour_t
  FindNonZero(const cv::Mat& img);

  size_t
  CountNonZero(const cv::Mat& img);
  
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

  geo2d::VectorArray<float>
  QPointOnCircle(const ::cv::Mat& img,
		 const geo2d::Circle<float>& circle,
		 const float pi_threshold=0.1,
		 const float supression=0);

  std::vector<geo2d::VectorArray<float> >
  QPointArrayOnCircleArray(const ::cv::Mat& img,
			   const geo2d::Vector<float>& center,
			   const std::vector<float>& radius_v,
			   const float pi_threshold=0.1,
			   const float supression=0);

  
  geo2d::VectorArray<float>
  RadialIntersections(const ::cv::Mat& polarimg,
		      const geo2d::Circle<float>& circle,
		      const size_t col,
		      const float pi_threshold,
		      const float supression=0);


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
  SquarePCA(const ::cv::Mat& img,
	    geo2d::Vector<float> pt,
	    float width, float height);

  double
  SquareR(const ::cv::Mat& img,
	  geo2d::Vector<float> pt,
	  float width, float height);
  
  void
  CorrectEdgeRectangle(const ::cv::Mat& img,
		       cv::Rect& rect,
		       int w,
		       int h);

  cv::Mat
  MaskImage(const cv::Mat& img,
	    const cv::Rect& rec,
	    int tol=0,
	    bool maskout=true);

  
}
#endif
