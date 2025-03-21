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
  
  uint thickness = 2;
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


  for(uint row=0; row < matc.rows; ++row) {
    matc.at<cv::Vec3b>(row,0) = {255,255,255};
    matc.at<cv::Vec3b>(row,matc.cols-1) = {255,255,255};
  }

  for(uint col=0; col < matc.cols; ++col) {
    matc.at<cv::Vec3b>(0,col) = {255,255,255};
    matc.at<cv::Vec3b>(matc.rows-1,col) = {255,255,255};
  }



  std::vector<float> rad_v = {10,20,30,40,50,60,70,80,90,100};

  for(auto& rad : rad_v)
    cv::circle(matc,center, rad,
	       cv::Scalar(255,0,0),2,8,0);

  auto pt_vv = larocv::QPointArrayOnCircleArray(mat,center,rad_v);

  for(size_t i=0; i<5; ++i) {
    std::cout << "@i=" << i << std::endl;
    auto then = Clock::now();
    pt_vv = larocv::QPointArrayOnCircleArray(mat,center,rad_v);
    auto now  = Clock::now();
    std::cout << "\t time " << stamp(now,then) << " ns" << std::endl;    
    then = Clock::now();
    pt_vv = larocv::OnCircleGroupsOnCircleArray(mat,center,rad_v);
    now  = Clock::now();
    std::cout << "\t time " << stamp(now,then) << " ns" << std::endl;    
  }


  pt_vv = larocv::QPointArrayOnCircleArray(mat,center,rad_v);

  for (auto pt_v : pt_vv)
    for(auto pt : pt_v)
      cv::circle(matc,cv::Point(pt),2,cv::Scalar(0,0,255),-1,8,0);
    
  std::stringstream ss;
  ss << "aho_" << thickness << ".png";
  cv::imwrite(ss.str().c_str(),matc);

  return 0;
}

  
