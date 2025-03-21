#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <vector>


void find_contours(const cv::Mat& img) {

  std::vector<std::vector<cv::Point_<int> > > ctor_v;
  std::vector<cv::Vec4i> cv_hierarchy_v;
  
  cv::findContours(img, ctor_v, cv_hierarchy_v, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  std::cout << "Found " << ctor_v.size() << " contours" << std::endl;
  for(const auto& ctor : ctor_v)
    std::cout << "\t... size " << ctor.size() << std::endl;
}

int main() {
  
  cv::Mat mat(485,485,CV_8UC1);
  mat.setTo(cv::Scalar(0));

  for(uint row=20;row<30;++row)
    for(uint col=200;col<300;++col)
      mat.at<uchar>(row,col) = (uchar)255;

  cv::imwrite("before.png",mat);
  find_contours(mat);
  cv::imwrite("after.png",mat);
  
  return 0;
}

  
