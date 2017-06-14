#ifndef __IMAGEMANAGER_CXX__
#define __IMAGEMANAGER_CXX__

#include "ImageManager.h"

namespace larocv {

  void ImageManager::push_back(const ::cv::Mat& img, const larocv::ImageMeta& meta)
  {
    ::cv::Mat res;
    img.copyTo(res);
    _mat_v.push_back(res);
    _meta_v.push_back(meta);
  }

  void ImageManager::emplace_back(cv::Mat&& img, larocv::ImageMeta&& meta)
  {
    _mat_v.push_back(std::move(img));
    _meta_v.push_back(std::move(meta));
  }

  ::cv::Mat& ImageManager::img_at(size_t index)
  {
    if(index >= _mat_v.size()) throw larbys("Requested image index exceeds available image count");
    return _mat_v[index];
  }

  ImageMeta& ImageManager::meta_at(size_t index)
  {
    if(index >= _meta_v.size()) throw larbys("Requested meta data index exceeds available image count");
    return _meta_v[index];
  }

  void ImageManager::clear()
  { _mat_v.clear(); _meta_v.clear(); }

}
#endif
