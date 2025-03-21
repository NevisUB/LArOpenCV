#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <vector>

#include "LArOpenCV/ImageCluster/AlgoFunction/ImagePatchAnalysis.h"

int main() {

  cv::Mat mat (485,485,CV_8UC1);

  mat.setTo(cv::Scalar(0));

  geo2d::Vector<float> center(256,228);
  

  for(short i=-20;i<20;++i)
    mat.at<uchar>(256+i,228+i) = (uchar)255;
  
  cv::imwrite("img.png",mat);
  
  std::cout << "R is... " << larocv::SquareR(mat,center,30,30) << std::endl;
  return 0;
}

  
