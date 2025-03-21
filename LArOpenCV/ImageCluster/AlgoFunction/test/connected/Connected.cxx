#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <vector>

#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"
#include "LArOpenCV/Core/laropencv_logger.h"


#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

long long stamp(std::chrono::time_point<Clock>& now,
		std::chrono::time_point<Clock>& then) {
  
  return std::chrono::duration_cast<std::chrono::nanoseconds>(now - then).count();
}

int main() {


  larocv::logger::get_shared().set(larocv::msg::kCRITICAL);

  uint row_m = 485*1;
  uint col_m = 485*1;

  cv::Mat mat (row_m,col_m,CV_8UC1);
  cv::Mat matc(row_m,col_m,CV_8UC3);

  mat.setTo(cv::Scalar(0));
  matc.setTo(cv::Scalar(0,0,0));

  geo2d::Vector<float> center(256,228);
  
  uint thickness = 20;

  for(uint row=225-thickness/2;row<225+thickness/2;++row) {
    for(uint col=0;col<485;++col) { 
      if (col>222 and col<262) continue;
      mat.at<uchar>(row,col) = (uchar)255;
      matc.at<cv::Vec3b>(row,col) = {255,255,255};
    }
  }

  for(uint row=0;row<485;++row) {
    for(uint col=256-thickness/2;col<256+thickness/2;++col) { 
      if (row>222 and row<262) continue;
      mat.at<uchar>(row,col) = (uchar)255;
      matc.at<cv::Vec3b>(row,col) = {255,255,255};
    }
  }

  // for(uint row=0; row < matc.rows; ++row) {
  //   matc.at<cv::Vec3b>(row,0) = {255,255,255};
  //   matc.at<cv::Vec3b>(row,matc.cols-1) = {255,255,255};
  // }

  // for(uint col=0; col < matc.cols; ++col) {
  //   matc.at<cv::Vec3b>(0,col) = {255,255,255};
  //   matc.at<cv::Vec3b>(matc.rows-1,col) = {255,255,255};
  // }


  auto pt1 = geo2d::Vector<float>(211,223);
  cv::circle(matc,cv::Point(pt1),2,cv::Scalar(0,0,255),-1,8,0);

  for(size_t row=0; row<485; row+=5) { 
    for(size_t col=0; col<485; col+=5) {
      auto pt2 = geo2d::Vector<float>(row,col);

      if (!larocv::NonZero(mat,pt2)) continue;

      auto con = larocv::Connected(mat,pt1,pt2,1);

      if (con)
	cv::circle(matc,cv::Point(pt2),2,cv::Scalar(255,0,0),-1,8,0);
      else
	cv::circle(matc,cv::Point(pt2),2,cv::Scalar(0,255,0),-1,8,0);
    }
  }
  
  std::stringstream ss;
  ss << "cacca_" << thickness << ".png";
  cv::imwrite(ss.str().c_str(),matc);

  return 0;
}

  
