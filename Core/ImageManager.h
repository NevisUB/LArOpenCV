/**
 * \file ImageManager.h
 *
 * \ingroup Core
 * 
 * \brief Class def header for a class ImageManager
 *
 * @author kazuhiro
 */

/** \addtogroup Core

    @{*/
#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>
#include "larbys.h"
#include "ImageMeta.h"
namespace larcv  {
  /**
     \class ImageManager
     A utility container class for multiple image objects with corresponding metadata per image.\n 
     In future this class will implement its own I/O structure so that analyzers can simply \n
     read/analyze/store image from ROOT file instead of converting from LArSoft/LArLite data file.
  */
  class ImageManager {
    
  public:
    
    /// Default constructor
    ImageManager(){}
    
    /// Default destructor
    ~ImageManager(){}

    /// Returns # images currently registered
    size_t size() const { return _mat_v.size(); }
    /// Register image by copying header
    void push_back(const ::cv::Mat& img, const larcv::ImageMeta& meta);
    /// Image getter
    ::cv::Mat& img_at(size_t index=0);
    /// Metadata getter
    larcv::ImageMeta& meta_at(size_t index=0);
    /// Clear data
    void clear();

  protected:
    /// Image container
    std::vector<cv::Mat> _mat_v;
    /// Metadata container
    std::vector<larcv::ImageMeta> _meta_v;
    
  };
}
#endif
/** @} */ // end of doxygen group 

