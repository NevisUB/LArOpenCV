/**
 * \file FillImageClusterVariables.h
 *
 * \ingroup ImageCluster
 * 
 * \brief Class def header for a class FillImageClusterVariables
 *
 * @author ariana Hackenburg
 */

/** \addtogroup ImageCluster

    @{*/
#ifndef FILLIMAGECLUSTERVARIABLES_H
#define FILLIMAGECLUSTERVARIABLES_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "opencv2/core/mat.hpp"
#include "ImageClusterTypes.h"

/**
   \class FillImageClusterVariables
   User defined class FillImageClusterVariables ... these comments are used to generate
   doxygen documentation!
 */

namespace larcv{

class FillImageClusterVariables{

public:

  /// Default constructor
  FillImageClusterVariables(){ _area_separation = 1850 ; _ratio_separation = 3.; }

  /// Default destructor
  ~FillImageClusterVariables(){}

  void Fill( ContourArray_t cv_contour ); //std::vector<::cv::Point>& cv_contour ) ;

  std::vector<float> Areas() { return _area_v ; }

  std::vector<float> Perimeters() { return _perimeter_v ; }

  std::vector<float> BBHeights() { return _bb_height_v ; }

  std::vector<float> BBWidths() { return _bb_width_v ; }

  std::vector<float> MaxContourWidths() { return _max_con_width_v ; }

  std::vector<float> MinContourWidths() { return _min_con_width_v; }

  ContourArray_t Satellites() { return _satellite_v; }

  ContourArray_t Showers() { return _shower_v; }

  ContourArray_t Tracks() { return _track_v; }

  void Clear() ;

private:

  std::vector<float> _area_v ;
  std::vector<float> _perimeter_v ;
  std::vector<float> _bb_height_v ;
  std::vector<float> _bb_width_v ;
  std::vector<float> _max_con_width_v ;
  std::vector<float> _min_con_width_v ;

  ContourArray_t _satellite_v ;
  ContourArray_t _shower_v ;
  ContourArray_t _track_v;

  float _area_separation ;
  float _ratio_separation ;

  };

}

#endif
/** @} */ // end of doxygen group 

