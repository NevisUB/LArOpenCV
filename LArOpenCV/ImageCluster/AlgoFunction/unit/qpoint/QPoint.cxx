#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <vector>

#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

int main() {

  cv::Mat mat (485,485,CV_8UC1);
  cv::Mat matc(485,485,CV_8UC3);

  mat.setTo(cv::Scalar(0));
  matc.setTo(cv::Scalar(0,0,0));

  geo2d::Vector<float> center(256,228);
  
  for(uint thickness=10; thickness<400; thickness+=10) {
  
    for(uint row=225-thickness/2;row<225+thickness/2;++row) {
      for(uint col=0;col<485;++col) { 
	mat.at<uchar>(row,col) = (uchar)255;
	matc.at<cv::Vec3b>(row,col) = {255,255,255};
      }
    }

    for(uint row=0;row<485;++row) {
      for(uint col=256-thickness/2;col<256+thickness/2;++col) { 
	mat.at<uchar>(row,col) = (uchar)255;
	matc.at<cv::Vec3b>(row,col) = {255,255,255};
      }
    }

  

    for(uint rad=1;rad<200;rad+=5) {

      float radius = rad;
      geo2d::Circle<float> circle(center,radius);

      auto pt_v = larocv::QPointOnCircle(mat,circle);


      cv::circle(matc,circle.center, circle.radius,
		 cv::Scalar(255,0,0),2,8,0);
  
      for(auto pt : pt_v)
	cv::circle(matc,cv::Point(pt),2,cv::Scalar(0,0,255),-1,8,0);
      
    }
    std::stringstream ss;
    ss << "out_" << thickness << ".png";
    cv::imwrite(ss.str().c_str(),matc);
  }
  
  return 0;
}

  
