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
#include "laropencv_base.h"
#include "larbys.h"
#include "ImageMeta.h"
#include "ROI.h"

namespace larocv  {
  /**
     \class ImageManager
     A utility container class for multiple image objects with corresponding metadata per image.\n 
     In future this class will implement its own I/O structure so that analyzers can simply \n
     read/analyze/store image from ROOT file instead of converting from LArSoft/LArLite data file.
  */
  class ImageManager : public laropencv_base{
    
  public:
    
    /// Default constructor
    ImageManager(const std::string name="ImageManager") : laropencv_base(name) {}
    
    /// Default destructor
    ~ImageManager(){}

    /// Returns # images currently registered
    size_t size() const { return _mat_v.size(); }
    /// Register image by copying header
    void push_back(const ::cv::Mat& img, const larocv::ImageMeta& meta);
    /// Regist image + meta + roi 
    void push_back(const ::cv::Mat& img, const larocv::ImageMeta& meta, const larocv::ROI& roi);
    /// Image getter
    ::cv::Mat& img_at(size_t index=0);
    /// Metadata getter
    larocv::ImageMeta& meta_at(size_t index=0);
    /// ROIdata getter
    larocv::ROI& roi_at(size_t index=0);
    /// Clear data
    void clear();

  protected:
    //public:
    /// Image container
    std::vector<cv::Mat> _mat_v;
    /// Metadata container
    std::vector<larocv::ImageMeta> _meta_v;
    /// ROI container
    std::vector<larocv::ROI> _roi_v;
    
  };
}
#endif
/** @} */ // end of doxygen group 

