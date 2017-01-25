#ifndef __IMAGEPATCHANALYSIS_H_
#define __IMAGEPATCHANALYSIS_H_

#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include <vector>
#include "Geo2D/Core/VectorArray.h"
#include "Geo2D/Core/Vector.h"
#include "Geo2D/Core/Circle.h"
#include "Geo2D/Core/Line.h"

namespace larocv {
  
  geo2d::VectorArray<float>
  QPointOnCircle(const ::cv::Mat& img, const geo2d::Circle<float>& circle,
		 const float pi_threshold=10);

  geo2d::Vector<float> MeanPixel(const cv::Mat& img, const geo2d::Vector<float>& center,
				 size_t range_x, size_t range_y, float pi_threshold=0);

  void FindEdges(const GEO2D_Contour_t& ctor,
		 geo2d::Vector<float>& edge1, geo2d::Vector<float>& edge2);

  geo2d::Line<float> SquarePCA(const ::cv::Mat& img,
			       geo2d::Vector<float> pt,
			       float width, float height);

  void CorrectEdgeRectangle(const ::cv::Mat& img, cv::Rect& rect,int w, int h);
}
#endif
