#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <vector>

#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

int main() {
  
  cv::Mat mat(485,485,CV_8UC1);
  mat.setTo(cv::Scalar(0));

  for(uint row=20;row<30;++row)
    for(uint col=0;col<485;++col)
      mat.at<uchar>(row,col) = (uchar)255;
      
  geo2d::Vector<float> center(256.463,28.4845);
  float radius=12;
  geo2d::Circle<float> circle(center,radius);

  auto mat_mask0 = larocv::MaskImage(mat,circle,0,false);
  auto mat_mask1 = larocv::MaskImage(mat,circle,0,true);
  
  cv::imwrite("orig_image.png",mat);
  cv::imwrite("mask0_image.png",mat_mask0);
  cv::imwrite("mask1_image.png",mat_mask1);
	      
  return 0;
}

  
