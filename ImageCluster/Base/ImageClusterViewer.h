/**
 * \file ImageClusterViewer.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class ImageClusterViewer
 *
 * @author kazuhiro
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef __IMAGECLUSTERVIEWER_H__
#define __IMAGECLUSTERVIEWER_H__

#include <iostream>
#include "ImageClusterTypes.h"
#include "Core/laropencv_base.h"
#include "FhiclLite/PSet.h"

namespace larocv {
  /**
     \class ImageClusterViewer
  */
  class ImageClusterViewer : public laropencv_base {
    
  public:
    
    /// Default constructor
    ImageClusterViewer(const std::string name="ImageClusterViewer");
    
    /// Default destructor
    ~ImageClusterViewer(){}

    /// Name accessor
    const std::string& Name() const { return _name; }
    
    /// Configuration method
    void Configure(const ::fcllite::PSet&);

    /// Execute viewer
    void Display(const ::cv::Mat& img,
		 const std::vector<larocv::ContourArray_t>& contours_v,
		 const std::vector<std::string>& window_name_v);

  private:

    std::string _name;      ///< self name
    size_t _display_width;  ///< Display width
    size_t _display_height; ///< Display height
    double _min_contour_area; ///< Min. area threshold for contour to be displaied 
  };
}
#endif
/** @} */ // end of doxygen group 

