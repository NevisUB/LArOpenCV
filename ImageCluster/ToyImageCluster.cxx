#ifndef __TOYIMAGECLUSTER_CXX__
#define __TOYIMAGECLUSTER_CXX__

#include "ToyImageCluster.h"
#include "Core/larbys.h"
#include <opencv2/imgproc/imgproc.hpp>
namespace larcv{

  void ToyImageCluster::_Configure_(const ::fcllite::PSet &pset)
  {
    _canny_params = pset.get<std::vector<double> >("CannyParams");
    if(_canny_params.size() != 3) throw larbys("CannyParams must be length 3 vector!");
  }

  ContourArray_t ToyImageCluster::_Process_(const larcv::ContourArray_t& clusters,
					    const ::cv::Mat& img,
					    larcv::ImageMeta& meta)
  {
    ::cv::Mat blurred_img;
    ::cv::blur( img, blurred_img, ::cv::Size(3,3) );
    ::cv::Canny(blurred_img,blurred_img,_canny_params[0],_canny_params[1],_canny_params[2]);
    ContourArray_t result;
    std::vector<cv::Vec4i> cv_hierarchy_v;
    //::cv::findContours(_canny_v[plane],cv_contour_v,cv_hierarchy_v,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
    ::cv::findContours(blurred_img,result,cv_hierarchy_v,
		       CV_RETR_EXTERNAL,
		       CV_CHAIN_APPROX_SIMPLE);
    //std::cout<<result.size()<<std::endl;
    return result;
  }

}
#endif
