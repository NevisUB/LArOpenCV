#ifndef __IMAGECLUSTERVIEWER_CXX__
#define __IMAGECLUSTERVIEWER_CXX__

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ImageClusterViewer.h"
#include "Core/larbys.h"
#include "BaseUtil.h"

namespace larocv {

  ImageClusterViewer::ImageClusterViewer(const std::string name)
    : laropencv_base(name)
    , _name(name)
    , _display_width(800)
    , _display_height(800)
    , _min_contour_area(125)
  {}
  
  void ImageClusterViewer::Configure(const ::fcllite::PSet& cfg)
  {
    LAROCV_DEBUG((*this)) << "Called\n";
    _display_width    = cfg.get<size_t>("Width");
    _display_height   = cfg.get<size_t>("Height");
    _min_contour_area = cfg.get<double>("MinContourArea");
    /*
    if(_display_ratio<0.5) {
      LAROCV_WARNING((*this)) << "Ratio cannot be smaller than 0.5 (setting to 0.5...)" << std::endl;
      _display_ratio = 0.5;
    }
    if(_display_ratio>2.0) {
      LAROCV_WARNING((*this)) << "Ratio cannot be larger than 2.0 (ssetting to 2.0...)" << std::endl;
      _display_ratio = 2.0;
    }
    */
    this->set_verbosity((msg::Level_t)(cfg.get<unsigned short>("Verbosity",3)));
    LAROCV_DEBUG((*this)) << "Return\n";
  }

  void ImageClusterViewer::Display(const ::cv::Mat& img,
				   const larocv::ContourArray_t& contours,
				   const std::vector<std::string>& display_name_v)
  {
    LAROCV_DEBUG((*this)) << "Called\n";
    
    // if(contours_v.size() != display_name_v.size()) {
    //   LAROCV_CRITICAL((*this)) << "Provided number of cluster sets and display names do not match!" << std::endl;
    //   throw larbys();
    // }

    // if(contours_v.empty()) {
    //   LAROCV_WARNING((*this)) << "Nothing to display..." << std::endl;
    //   return;
    // }
    
    //
    // How it works: Make all images to display in a vector then call imshow at once later.
    // 0) Register original image w/o any contour
    // 1) Find BoundingBox per set of contours (i.e. per algorithm) to display & prepare specific cv::Mat
    // 2) Display
    //
    
    // 0) Register original image
    std::vector<cv::Mat> result_image_v;
    ::cv::Mat orig_image(img.size(),CV_8UC3);
    //::cv::Mat orig_image;
    //img.copyTo(orig_image);
    double minVal;
    double maxVal;
    minMaxLoc(img, &minVal, &maxVal);
    for(int j=0;j<img.rows;j++) {
      for (int i=0;i<img.cols;i++) {
	double q = img.at<unsigned char>(j,i);
	q *= 255.*255.*255./maxVal;
	int b = ((int)q)/(255*255);
	int g = ((int)q - b*255*255)/(255);
	int r = ((int)q - b*255*255 - g*255);
	orig_image.at<unsigned char>(j,i,0) = (unsigned char)r;
	orig_image.at<unsigned char>(j,i,1) = (unsigned char)g;
	orig_image.at<unsigned char>(j,i,2) = (unsigned char)b;
      }
    }
    LAROCV_DEBUG((*this))<<minVal<<" => "<<maxVal<<std::endl;

    //::cv::cvtColor(orig_image,orig_image,CV_GRAY2RGB);
    //size_t imshow_width  = (orig_image.rows > _display_width  ? _display_width  : orig_image.rows);
    //size_t imshow_height = (orig_image.cols > _display_height ? _display_height : orig_image.cols);

    size_t imshow_width  = _display_width;
    size_t imshow_height = _display_height;
    LAROCV_INFO((*this)) << "Original size: " << orig_image.rows << " : " << orig_image.cols
			 << " ... "
			 << "Resizing: " << imshow_width << " : " << imshow_height << std::endl;

    if(!orig_image.rows || !orig_image.cols) return;
    if(!imshow_width || !imshow_height) return;

    ::cv::resize(orig_image,orig_image,::cv::Size(imshow_width,imshow_height),0,0,::cv::INTER_AREA);
    //::cv::bitwise_not (orig_image,orig_image);
    result_image_v.emplace_back(orig_image);

    // 1) Find BoundingBox per set of contours (i.e. per algorithm) to display & prepare specific cv::Mat
    //for(auto const& contours : contours_v) {

    LAROCV_DEBUG((*this)) << "Creating images for " << contours.size() << " contours..." << std::endl;
      
    // Find bounding box limits
    auto rect = BoundingBox(contours,_min_contour_area);
    LAROCV_DEBUG((*this)) << "Bounding box: "
			  << rect.x << " => " << rect.x + rect.width
			  << " : "
			  << rect.y << " => " << rect.y + rect.height << std::endl;

    // Prepare Mat
    ::cv::Mat result_image;
    ::cv::cvtColor(img,result_image,CV_GRAY2RGB);

    // Draw contours
    ::cv::RNG rng;
    for(size_t cindex=0; cindex < contours.size(); ++cindex) {
      ::cv::Scalar color = ::cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
      drawContours( result_image, contours, cindex, color, 2, 8, ::cv::noArray(), 0, ::cv::Point() );
    }

    // Only show relevant area
    result_image = result_image(rect).clone();      
    //imshow_width  = (result_image.rows > _display_width  ? _display_width  : result_image.rows);
    //imshow_height = (result_image.cols > _display_height ? _display_height : result_image.cols);
    LAROCV_INFO((*this)) << "Original size: " << img.rows << " : " << img.cols
			 << " ... "
			 << "Bounded size : " << result_image.rows << " : " << result_image.cols
			 << " ... "
			 << "Resizing: " << imshow_width << " : " << imshow_height << std::endl;
    if(imshow_width && imshow_height && result_image.rows && result_image.cols) {
      ::cv::resize(result_image,result_image,::cv::Size(imshow_width,imshow_height),0,0,::cv::INTER_AREA);
      ::cv::bitwise_not(result_image,result_image);
    }
    result_image_v.emplace_back(result_image);
    //}

    // 2) Display
    for(size_t i=0; i<result_image_v.size(); ++i) {
      std::string name = "Original";
      if(i) name = display_name_v[i-1];

      auto const& image = result_image_v[i];
      if(image.rows>0 && image.cols>0) {
	::cv::namedWindow(name, CV_WINDOW_NORMAL);
	::cv::imshow(name, image);
      }
    }

    cvWaitKey(0);

    for(size_t i=0; i<result_image_v.size(); ++i) {
      std::string name = "Original";
      if(i) name = display_name_v[i-1];
      cvDestroyWindow(name.c_str());
    }
    LAROCV_DEBUG((*this)) << "Return\n";
  }
}

#endif
