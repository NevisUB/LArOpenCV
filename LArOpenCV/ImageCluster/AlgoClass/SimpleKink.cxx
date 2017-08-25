#ifndef SIMPLEKINK_CXX
#define SIMPLEKINK_CXX

#include "SimpleKink.h"
#include "Geo2D/Core/Geo2D.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "LArOpenCV/Core/laropencv_base.h"
#include "LArOpenCV/Core/larbys.h"
#include "LArOpenCV/ImageCluster/Base/BaseUtil.h"

#include "LArOpenCV/ImageCluster/Base/ImageClusterTypes.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/LArProperties.h"

using larocv::larbys;

namespace larocv {

  SimpleKink::SimpleKink()
  {
    _subpath_step_size = 5;
    _min_path_ratio    = 1000000;
    _chunkiness        = 30;
  }
  
  void SimpleKink::Configure(const Config_t &pset) {
    _subpath_step_size = pset.get<uint>("SimpleKinkSubPathSize",5);
    _min_path_ratio    = pset.get<float>("MinPathRatio",1000000);
    _chunkiness        = pset.get<uint>("Chunkiness",30);
  }  

  std::vector<geo2d::Vector<float>> SimpleKink::FindInflections(const GEO2D_Contour_t &ctor, const cv::Mat &img) {

    std::vector<geo2d::Vector<float>> inflections;
    
    cv::Mat mask(img.size(),img.type(),cv::Scalar(0));

    cv::drawContours(mask,GEO2D_ContourArray_t(1,ctor) , 0 , cv::Scalar(255), -1 , cv::LINE_8);

    std::vector<cv::Point> insidePoints;
    cv::findNonZero(mask,insidePoints);

    float maxRatio  = 0;
    float thisRatio = 0;
    float potKinkX;
    float potKinkY;
  
    for(uint i=_subpath_step_size; i<insidePoints.size(); ++i) {

      float pathLen = 0;
      float crowLen = 0;
      for(uint j = 0; j<_subpath_step_size; ++j) {
	auto const& pt0 = insidePoints.at(i+j-_subpath_step_size);
	auto const& pt1 = insidePoints.at(i+j-_subpath_step_size + 1);
	
	float p0x = (float)pt0.x;
	float p1x = (float)pt1.x;
	float p0y = (float)pt0.y;
	float p1y = (float)pt1.y;

	pathLen += sqrt( pow(p0x-p1x ,2) + pow(p0y-p1y,2) );
      }

      
      auto const& pt_start = insidePoints.at(i-_subpath_step_size);
      auto const& pt_end   = insidePoints.at(i);

      float pt_startX = (float)pt_start.x;
      float pt_startY = (float)pt_start.y;
      float pt_endX   = (float)pt_end.x;
      float pt_endY   = (float)pt_end.y;
 
      crowLen = sqrt( pow(pt_startX-pt_endX , 2) + pow(pt_startY-pt_endY , 2) );
     
      thisRatio = pathLen / crowLen;

      if (thisRatio > maxRatio){
	maxRatio = thisRatio;
	potKinkX = (pt_endX+pt_startX)/2.0;
	potKinkY = (pt_endY+pt_startY)/2.0;
      }

      if (i%_chunkiness == 0){
	if (maxRatio > _min_path_ratio){
	  geo2d::Vector<float> potentialKink(potKinkX,potKinkY);
	  inflections.emplace_back(potentialKink);
	}
	maxRatio = 0;
      }
    }

    return inflections;
  }
}

#endif
