#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <vector>

#include "LArOpenCV/ImageCluster/AlgoFunction/Contour2DAnalysis.h"

int main() {
  
  cv::Mat mat(485,485,CV_8UC1);
  mat.setTo(cv::Scalar(0));

  for(uint row=200;row<300;++row)
    for(uint col=200;col<300;++col)
      mat.at<uchar>(row,col) = (uchar)255;


  auto mat_copy = mat.clone();
  
  cv::imwrite("orig_image.png",mat_copy);

  //make the super contour
  larocv::GEO2D_Contour_t super_ctor(4);
  super_ctor[0]=cv::Point(200,200);
  super_ctor[1]=cv::Point(200,300);
  super_ctor[2]=cv::Point(300,300);
  super_ctor[3]=cv::Point(300,200);

  cv::drawContours(mat_copy, larocv::GEO2D_ContourArray_t(1,super_ctor), -1, cv::Scalar(150), 5, cv::LINE_8);
  
  //make the targe contour
  larocv::GEO2D_Contour_t target_ctor(4);
  target_ctor[0]=cv::Point(225,225);
  target_ctor[1]=cv::Point(225,274);
  target_ctor[2]=cv::Point(274,274);
  target_ctor[3]=cv::Point(274,225);

  //if you want you can shift the target contour around
  for(auto& pt: target_ctor) pt+=cv::Point(-10,-10);
  
  cv::drawContours(mat_copy, larocv::GEO2D_ContourArray_t(1,target_ctor), -1, cv::Scalar(100), 3, cv::LINE_8);
  cv::imwrite("ctor_image.png",mat_copy);

  auto px_frac = larocv::PixelFraction(mat,super_ctor,target_ctor);

  std::cout << "Pixel fraction: " << px_frac << " should be 0.25" << std::endl;
  
  return 0;
}

  
